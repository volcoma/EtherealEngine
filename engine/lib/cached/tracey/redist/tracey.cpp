/* Tracey, a simple and lightweight memory leak detector
 * Copyright (c) 2011,2012,2013,2014 r-lyeh
 * ZLIB/LibPNG licensed.

 * Features:
 * - tiny
 * - clean
 * - callstack based
 * - lazy. should work with no source modification
 * - macroless API. no new/delete macros
 * - embeddable. just link it against your project
 * - C hooks (optional) (win32 only)
 * - C++ hooks (optional)
 * - web server (optional)

 * To do:
 * - support for C hooks on non-win archs:
 *   - http://stackoverflow.com/questions/262439/create-a-wrapper-function-for-malloc-and-free-in-c
 *   - http://src.chromium.org/svn/trunk/src/tools/memory_watcher/memory_hook.cc
 *   - void __f () { } // Do something.
 *     void f () __attribute__ ((weak, alias ("__f")));
 *   - or http://man7.org/linux/man-pages/man3/malloc_hook.3.html
 *   - as seen in https://github.com/emeryberger/Heap-Layers/blob/master/wrappers/gnuwrapper.cpp
 * - memory manager checks
 *   - Read before allocating memory
 *   - Write before allocating memory
 *   - Read before beginning of allocation
 *   - Write before beginning of allocation
 *   - Read after end of allocation
 *   - Write after end of allocation
 *   - Read after deallocation
 *   - Write after deallocation
 *   - Failure to deallocate memory
 *   - Deallocating memory twice
 *   - Deallocating nonallocated memory
 *   - Zero-size memory allocation
 * - optional pools for microallocations
 * - reports per:
 *   - symbol: namespace/class/function
 *   - author: blame, candidates (caller and callee for highest apparitions founds on blame(symbol(ns/cl/fn)))
 *   - location: filename, directory
 *   - origin: module/thread
 *   - wasted size, % total, hits
 * - ignores per project/session:
 *   - symbol: namespace/class/function
 *   - author: blame, candidates
 *   - location: filename, directory
 *   - origin: module/thread
 * - versioning support (svn/git...)
 * - cloud account linking support (github/gitorius/etc):
 *   - auto commit/pull symbol maps
 *   - auto commit/pull settings
 *   - auto commit reports
 * - auto updates on ui: load webserver templates from official github repo

 * References:
 * - http://www.codeproject.com/KB/cpp/allocator.aspx
 * - http://drdobbs.com/cpp/184403759

 * - rlyeh ~~ listening to Long Distance Calling / Metulsky Curse Revisited
 */

// A few speed tweaks before loading STL on MSVC

#ifdef _SECURE_SCL
#undef _SECURE_SCL
#endif
#define _SECURE_SCL 0
#ifdef _HAS_ITERATOR_DEBUGGING
#undef _HAS_ITERATOR_DEBUGGING
#endif
//#define _HAS_ITERATOR_DEBUGGING 0

// Include C, then C++

#include <cassert>
#include <cctype>
// #include <cstddef> // (stddef.h fails on ArchLinux w/ clang 3.4)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <new>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// Our API {
#   include "tracey.hpp"
// }

// external; tree implementation {
#   include "deps/oak/oak.hpp"
// }

// external; macros, OS utils. Here is where the fun starts {
#   define HEAL_MAX_TRACES kTraceyMaxStacktraces
#   define heal tracey_heal
#   include "deps/heal/heal.cpp" // also includes heal.hpp
// }

// mutexes and threads
#if $on($cpp11)
#include <mutex>
#include <thread>
#else
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
namespace std {
    using boost::thread;
    using boost::recursive_mutex;
}
#endif

// external; route66 uses c++11
#if kTraceyWebserverPort
#   include "deps/route66/route66.cpp"  // also includes route66.hpp
#endif

// checks: todo
// if /MD or /MDd and kTraceyReportWildPointers warn user "not a good idea"
#if kTraceyHookLegacyCRT
    $warning( "<tracey/tracey.cpp> says: kTraceyHookLegacyCRT option ignored. CRT hooking not supported on this platform.")
#   undef  kTraceyHookLegacyCRT
#   define kTraceyHookLegacyCRT 0
#endif


namespace tracey
{
    static void webmain( void * );
    static void hotkeymain( void * );

    typedef heal::sfstring string;
    typedef heal::sfstrings strings;

    std::string human( size_t bytes ) {
        /**/ if( bytes >= 1024 * 1024 * 1024 ) return tracey::string("\1 GB", bytes / (1024 * 1024 * 1024));
        else if( bytes >=   16 * 1024 * 1024 ) return tracey::string("\1 MB", bytes / (       1024 * 1024));
        else if( bytes >=          16 * 1024 ) return tracey::string("\1 KB", bytes / (              1024));
        else                                   return tracey::string("\1 bytes", bytes );
    }

    struct branch {
        size_t hits;
        size_t size;
        branch() : hits(0), size(0) {
        }
        branch( const branch &other ) {
            operator=( other );
        }
        branch &operator =( const branch &other ) {
            if( this != &other ) {
                hits = other.hits;
                size = other.size;
            }
            return *this;
        }
        branch &operator +=( const branch &other ) {
            this->size += /*(other.hits ? other.hits : 1) **/ other.size;
            return *this;
        }
        bool operator<( const double t ) const {
            return size < t;
        }

        std::string str( double total ) const {
            tracey::string os;
            int pct = int((size * 100.0) / total);
            /**/ if( pct >= 100 ) os << tracey::string(   "\1%", pct ) << " .. " << tracey::human( size );
            else if( pct >=  10 ) os << tracey::string(  "0\1%", pct ) << " .. " << tracey::human( size );
            else                  os << tracey::string( "00\1%", pct ) << " .. " << tracey::human( size );

            if( hits > 1 ) os << " ; " << hits << " hits";
            return os;
        }
    };
}

namespace tracey {

    //#include <map>
    //#include <iostream>

    // tree class
    // [] is read-only
    // () is read-writeable (so is <<)
    typedef void *K;
    typedef branch V;
    typedef oak::tree<K,V> tree;

    // for tree walking
    struct chopper {
        bool operator()( tracey::tree &tree, tracey::tree::iterator &it ) const {
            if( it->second.get() < kTraceyTruncateBranchesSmallerThan ) {
                tree.erase( it->first );
                return false;
            } else {
                return true;
            }
        }
    };

    template<typename U>
    std::string print( const tree &self, double total, const tracey::string &fmt1234, const tracey::string &sorted_by, bool reversed, const std::map<K,U> &tmap, unsigned depth ) {
        std::map<std::string,tracey::strings> list;
        std::string tabs( depth, kTraceyCharTab[0] );
        if( !tmap.empty() ) {
            for( tree::const_iterator it = self.begin(), end = self.end(); it != end; ++it  ) {
                tracey::string key( sorted_by, tmap.find( it->first )->second, it->second.get().str(total), tabs, self.size() );
                tracey::string value( fmt1234, tmap.find( it->first )->second, it->second.get().str(total), tabs, self.size() );
                list[ key ].push_back( value );
                list[ key ].push_back( print( it->second, total, fmt1234, sorted_by, reversed, tmap, depth + 1 ) );
            }
        } else {
            for( tree::const_iterator it = self.begin(), end = self.end(); it != end; ++it  ) {
                tracey::string key( sorted_by, it->first, it->second.get().str(total), tabs, self.size() );
                tracey::string value( fmt1234, it->first, it->second.get().str(total), tabs, self.size() );
                list[ key ].push_back( value );
                list[ key ].push_back( print( it->second, total, fmt1234, sorted_by, reversed, tmap, depth + 1 ) );
            }
        }
        std::string out;
        if( !reversed ) {
            for( std::map<std::string,tracey::strings>::const_iterator it = list.begin(), end = list.end(); it != end; ++it ) {
                out += it->second.flat();
            }
        } else {
            for( std::map<std::string,tracey::strings>::const_reverse_iterator it = list.rbegin(), end = list.rend(); it != end; ++it ) {
                out += it->second.flat();
            }
        }
        return out;
    }

    template<typename U>
    std::string print( const tree &self, double total, const std::map<K,U> &tmap, const tracey::string &format = "{tabs}[{size}] {key} ({value})\n", const tracey::string &sorted_by = "{key}", bool reversed = false ) {
        tracey::string fmt = format
        .replace("{key}",   "\1")
        .replace("{value}", "\2")
        .replace("{tabs}",  "\3")
        .replace("{size}",  "\4");
        tracey::string sort = sorted_by
        .replace("{key}",   "\1")
        .replace("{value}", "\2")
        .replace("{tabs}",  "\3")
        .replace("{size}",  "\4");
        return print( self, total, fmt, sort, reversed, tmap, 0 );
    }

    std::string print( const tree &self, double total, const tracey::string &format = "{tabs}[{size}] {key} ({value})\n", const tracey::string &sorted_by = "{key}", bool reversed = false ) {
        std::map<K, K> dummy;
        return print( self, total, dummy, format, sorted_by, reversed );
    }

    // callstack, demangle, lookup
    using namespace heal;
}

namespace tracey
{
    namespace
    {
        volatile size_t timestamp_id = 0;
        struct stats_t {
            size_t usage, usage_peak, num_leaks, leak_peak, overhead;
            stats_t() : usage(0), usage_peak(0), num_leaks(0), leak_peak(0), overhead(0) {}
            std::string str() const {
                return tracey::string("highest peak: \1 total, \2 greatest peak // \3 allocs in use: \4 + overhead: \5 = total: \6",
                                    human(usage_peak), human(leak_peak), num_leaks, human(usage), human(overhead), human( usage + overhead ) );
            }
        } stats;

        size_t create_id() {
            static size_t id = 0;
            return ++id;
        }

        bool view_report( const std::string &html ) {
            $windows( return std::system( tracey::string("start \1", html).c_str() ), true );
            $apple( return std::system( tracey::string("open \1", html).c_str() ), true );
            $linux( return std::system( tracey::string("xdg-open \1", html).c_str() ), true );
            return false;
        }

        std::string get_temp_pathfile() {
            $windows(
                std::string path = std::string( std::getenv("TEMP") ? std::getenv("TEMP") : "." ) + std::tmpnam(0);
                while( path.size() && path.at( path.size() - 1 ) == '.' ) path.resize( path.size() - 1 );
                return path;
            )
            $welse(
                return std::string() + std::tmpnam(0);
            )
        }
    }

    namespace detail
    {
        struct leak {
            size_t id, size;
            const void *addr;
            tracey::callstack cs;

            leak() : size(0), id(0), addr(0)
            {}

            void wipe() {
                id = create_id();
                cs = tracey::callstack();
                size = 0;
                addr = 0;
            }

            ~leak() {
                wipe();
            }
        };
    }

    namespace
    {
        using namespace tracey::detail;

        typedef std::vector< const leak * > leaks;

        // recursive mutex
        std::recursive_mutex *mutex = 0;

        // hard on/off switch
        static const    bool kTraceyEnabledHard = kTraceyEnabled;
        // soft on/off switch
        static volatile bool kTraceyEnabledSoft = true;

        class container : public std::map< const void *, leak, std::less< const void * > > //, tracey::malloc_allocator< std::pair< const void *, leak * > > >
        {
            public:

            container() 
            {
                 mutex = new std::recursive_mutex();
            }

            ~container() {
                mutex = 0;

                if( kTraceyReportOnExit && kTraceyEnabledSoft ) {
                    view_report( _report() );
                }
                
                // We should clear the memory involved in all allocations but after all, we are a memory tracker and we should be the very
                // last thing to deallocate in any program. So we don't do it and we save some time for developing when exiting the app.
                // Silent & quick exit. Not a requirement in all architectures but who knows.
                kTraceyDie( __LINE__ );
            }

            void _clear() {

                for( iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                    it->second.wipe();
                }

                this->clear();
            }

            leaks collect_leaks( size_t *wasted ) const {
                leaks list;
                *wasted = 0;
                for( const_iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                    const tracey::detail::leak &L = it->second;
                    if( L.addr && L.size && L.id >= timestamp_id ) {
                        *wasted += L.size;
                        list.push_back( &L );
                    }
                }
                return list;
            }

            std::string _report() const {

                std::string logfile = get_temp_pathfile() + "xxx-tracey.html";

                kTraceyPrintf( "%s", tracey::string( "<tracey/tracey.cpp> says: summary: \1" kTraceyCharLinefeed, stats.str() ).c_str() );
                kTraceyPrintf( "%s", tracey::string( "<tracey/tracey.cpp> says: creating report: \1" kTraceyCharLinefeed, logfile).c_str() );
                kTraceyfFile *fp = kTraceyfOpen( logfile.c_str(), "wb" );

                // this code often runs at the very end of a program cycle (even when static memory has been deallocated)
                // so, avoid using global C++ objects like std::cout/cerr as much as possible; otherwise crashes may happen

                $windows(
                    AllocConsole();
                )

                // Find leaks
                kTraceyPrintf( "%s", tracey::string("<tracey/tracey.cpp> says: filtering leaks..." kTraceyCharLinefeed).c_str() );
                size_t wasted, n_leak;
                leaks filtered = collect_leaks( &wasted );
                n_leak = filtered.size();
                kTraceyPrintf( "%s", tracey::string("<tracey/tracey.cpp> says: found \1 leaks wasting \2" kTraceyCharLinefeed, n_leak, human(wasted)).c_str() );

                // Calc score
                double leaks_pct = this->size() ? n_leak * 100.0 / this->size() : 0.0;
                std::string score = "perfect!";
                if( leaks_pct >  0.00 ) score = "excellent";
                if( leaks_pct >  1.25 ) score = "good";
                if( leaks_pct >  2.50 ) score = "poor";
                if( leaks_pct >  5.00 ) score = "mediocre";
                if (leaks_pct > 10.00 ) score = "lame";

                // Header
                kTraceyfPrintf( fp, "%s", tracey::string( "<html><body><xmp>" ).c_str() );
                kTraceyfPrintf( fp, "%s", tracey::string( "<tracey/tracey.cpp> says: generated with \1 (\2)" kTraceyCharLinefeed, tracey::version(), tracey::url() ).c_str() );
                if( tracey::lookup(url) == "????" )
                kTraceyfPrintf( fp, "%s", tracey::string( "<tracey/tracey.cpp> says: failed to decode symbols!! Is debug information available?" $msvc(" Are .PDB files available?") kTraceyCharLinefeed ).c_str() );
                kTraceyfPrintf( fp, "%s", tracey::string( "<tracey/tracey.cpp> says: best viewed on foldable text editor (like SublimeText2) with tabs=2sp and no word-wrap" kTraceyCharLinefeed ).c_str() );
                kTraceyfPrintf( fp, "%s", tracey::string( "<tracey/tracey.cpp> says: \1, \2 leaks found; \3 wasted ('\4' score)" kTraceyCharLinefeed, !n_leak ? "ok" : "error", n_leak, human(wasted), score ).c_str() );
                kTraceyfPrintf( fp, "%s", tracey::string( "<tracey/tracey.cpp> says: summary: \1" kTraceyCharLinefeed, stats.str() ).c_str() );
                kTraceyfPrintf( fp, "%s", tracey::string( "<tracey/tracey.cpp> says: report filename: \1" kTraceyCharLinefeed, logfile).c_str() );

                // Body
                // Get all frame addresses involved in all leaks
                // Also, create a tree of frames; so we will take decisions from above by examining node weights (@todo)
                kTraceyPrintf( "%s", tracey::string("<tracey/tracey.cpp> says: creating trees of frames..."  kTraceyCharLinefeed).c_str() );
                std::set< void * > set;
                tracey::tree tree;
                tree[ (void *)((~0)-0) ]; // bottom-top branch
                tree[ (void *)((~0)-1) ]; // top-bottom branch
                for( leaks::const_iterator it = filtered.begin(), end = filtered.end(); it != end; ++it ) {
                    const leak &L = **it;
                    const tracey::callstack &callstack = L.cs;
                    tracey::tree *_tree = &tree[(void *)((~0)-0)];
                    tracey::tree *_tree_inv = &tree[(void *)((~0)-1)];
                    if( !callstack.frames.size() ) continue;

                    for( unsigned i = 0, start = kTraceyStacktraceSkipBegin, end = callstack.frames.size() - 1 - kTraceyStacktraceSkipEnd; start+i <= end; ++i ) {

                        double size;
                        size = (start+i == end ? L.size : 0);
                        (*_tree)[ callstack.frames[start + i] ].get().size += size;
                        (*_tree)[ callstack.frames[start + i] ].get().hits ++;
                        //(*_tree)[ callstack.frames[start + i] ].get().total = 100.0 * size / wasted;
                        _tree = &(*_tree)[ callstack.frames[start + i] ];
                        set.insert( callstack.frames[start + i] );

                        size = (end - i == start ? L.size : 0);
                        (*_tree_inv)[ callstack.frames[end - i] ].get().size += size;
                        (*_tree_inv)[ callstack.frames[end - i] ].get().hits ++;
                        //(*_tree_inv)[ callstack.frames[end - i] ].get().total = 100.0 * size / wasted;
                        _tree_inv = &(*_tree_inv)[ callstack.frames[end - i] ];
                        set.insert( callstack.frames[end - i] );
                    }
                }

                // Some apps are low on memory in here, so we free memory as soon as possible
                filtered = leaks();

                if( !set.size() ) {
                    if( n_leak ) {
                        kTraceyPrintf( "%s", tracey::string("<tracey/tracey.cpp> says: error! failed to resolve symbols." $msvc(" Are PDB files available?") kTraceyCharLinefeed).c_str() );
                    }
                } else {
                    kTraceyPrintf( "%s", tracey::string("<tracey/tracey.cpp> says: resolving \1 unique frames..." kTraceyCharLinefeed, set.size()).c_str() );
                    // convert set of unique frames into array of frames
                    tracey::callstack cs;
                    std::vector< void * > &frames = cs.frames;
                    frames.reserve( set.size() );
                    for( std::set< void * >::iterator it = set.begin(), end = set.end(); it != end; ++it ) {
                        frames.push_back( *it );
                    }
                    tracey::strings symbols = cs.unwind();
                    std::map< void *, std::string > translate;
                    {
                        if( frames.size() != symbols.size() ) {
                            kTraceyfPrintf( fp, "%s", tracey::string("<tracey/tracey.cpp> says: error! cannot resolve all frames (\1 vs \2)!" kTraceyCharLinefeed, frames.size(), symbols.size() ).c_str() );
                            for( unsigned i = 0, end = frames.size(); i < end; ++i ) {
                                translate[ frames[i] ] = tracey::string("\1", frames[i]);
                            }
                        } else {
                            for( unsigned i = 0, end = frames.size(); i < end; ++i ) {
                                translate[ frames[i] ] = symbols[ i ];
                            }
                        }

                        // Create a tree report, if possible
                        kTraceyPrintf( "%s", tracey::string("<tracey/tracey.cpp> says: converting tree of frames into tree of symbols..." kTraceyCharLinefeed).c_str() );
                        translate[ (void *)((~0)-1) ] = "begin";
                        translate[ (void *)((~0)-0) ] = "end";
                        tree.refresh();
                        //std::cout << tree << std::endl;

                        // truncate branches lower than user-defined percentage (it helps reducing log size)
                        // note: kTraceyTruncateBranchesSmallerThan defaults to 0% (~do not truncate branches, show all leaks)

                        kTraceyPrintf( "%s", tracey::string("<tracey/tracey.cpp> says: flattering tree of symbols..." kTraceyCharLinefeed).c_str() );
                        if( kTraceyTruncateBranchesSmallerThan > 0 ) {
                            tree.walk<tracey::chopper>();
                        }

                        kTraceyPrintf( "%s", tracey::string("<tracey/tracey.cpp> says: dumping tree log..." kTraceyCharLinefeed).c_str() );
                        kTraceyfPrintf( fp, "%s", tracey::print(tree, wasted, translate, "{tabs}[{size}] ({value}) {key}\n", "{value}", true).c_str() );
                    }
                }

                // Footer
                kTraceyfPrintf( fp, "%s", tracey::string( "</xmp></body></html>" ).c_str() );
                kTraceyfClose( fp );

                return logfile;
            }
        };

        void *tracer( void *ptr, size_t &size );

        container &init() {
            //static unsigned char buy[ sizeof( container ) ];
            //static container *map = new (buy) container();
            static $cpp11(std::unique_ptr) $cpp03(std::auto_ptr)<container> map( new container() );

            static bool once = false; if(! once ) { once = true;
                kTraceyPrintf( "%s", tracey::settings().c_str() );
                if( 1 ) {
                    webmain( 0 );
                }
                if( 1 ) {
                    volatile bool sitdown = false;
                    std::thread(tracey::hotkeymain, (void *) &sitdown).detach();
                    while( !sitdown ) {
                    $windows( Sleep( 1000 ) );
                    $welse( sleep( 1 ) );
                    }
                }
                // Construct internals of tracer (static initializers)
                // size_t dummy = 0;
                // tracer( 0, dummy );
            }
            return *map;
        }

        void *tracer( void *ptr, size_t &size )
        {
            if( !ptr )
                return size = 0, ptr;

            if( !kTraceyEnabledHard )                       // hard on/off switch
                return size = 0, ptr;

            if( !kTraceyEnabledSoft && (size < (~0) - 4) )  // soft on/off switch; only for mallocs & frees
                return size = 0, ptr;

#if         kTraceyHookLegacyCRT
            // do nothing
#else
            static bool initialized = true;
            if( !initialized )
                return size = 0, ptr;
            static container *init = 0;
            static const bool reinit = (initialized = false, init = &tracey::init(), initialized = true);
            static container &map = *init;
#endif

            if ( !mutex )
                return size = 0, ptr;

            // threads will lock here till the slot is free.
            // threads will return on recursive locks.

            static $tls(bool) acquired = false;
            mutex->lock();
            if( acquired ) {
                mutex->unlock();
                return size = 0, ptr;
            } else {
                acquired = true;               
            }

            // ready
            container::iterator it = map.find( ptr );
            bool found = ( it != map.end() );

            if( size == ~0 || size == 0 )
            {
                if( found )
                {
                    leak &L = it->second;
                    stats.overhead -= L.cs.space();
                    stats.usage -= L.size;
                    stats.num_leaks--;
                    L.wipe();
                }
                else
                {
                    // 1st) wild pointer deallocation found; warn user
                    if( kTraceyReportWildPointers )
                        kTraceyPrintf( "%s", (tracey::string( "<tracey/tracey.cpp> says: Error, wild pointer deallocation." kTraceyCharLinefeed ) +
                            tracey::callstack( true ).flat( kTraceyCharTab "\1) \2" kTraceyCharLinefeed, kTraceyStacktraceSkipBegin) ).c_str() );

                    // 2nd) normalize ptr for further deallocation (deallocating null pointers is ok)
                    ptr = 0;
                }
            }
            else
            if( size == (~0) - 1 )
            {
                int code = *((int*)ptr);
                //ptr = 0;
                if( code == 1 ) {
                    map._clear();
                    stats = stats_t();
                    timestamp_id = create_id();
                }

                if( code == 2 ) { *((stats_t*)ptr) = stats; };
                if( code == 3 ) (void)0;
                if( code == 4 ) (void)0;
            }
            else
            if( size == (~0) - 2 )
            {
                static char placement[ sizeof(std::string) ];
                static std::string *log = new ((std::string *)placement) std::string();
                *log = map._report();
                ptr = (void *)log;
            }
            else
            if( size == (~0) - 3 )
            {
                std::string &log = *((std::string *)(ptr));
                view_report( log );
            }
            else
            if( size == (~0) - 4 )
            {
                static char placement[ sizeof(std::string) ];
                static std::string *log = new ((std::string *)placement) std::string();
                *log = stats.str();
                ptr = (void *)log;
            }
            else
            {
                if( found ) {
                    if( kTraceyReportDoubleAllocations ) {
                        kTraceyPrintf( "%s", (tracey::string( "<tracey/tracey.cpp> says: Error, double pointer allocation. This should never happen" kTraceyCharLinefeed ) +
                            tracey::callstack( true ).flat( kTraceyCharTab "\1) \2" kTraceyCharLinefeed, kTraceyStacktraceSkipBegin) ).c_str() );
                    }
                    // kTraceyAssert( !map[ptr].first );
                    // kTraceyAssert( !map[ptr].second );
                }
                kTraceyAssert( size > 0 );

                // create a leak and (re)insert it into map
                tracey::detail::leak &leak = (map[ptr] = map[ptr]);
                leak.wipe();
                leak.cs.save();
                leak.addr = ptr;
                leak.size = size;

                // update stats
                stats.num_leaks++;
                stats.usage += size;
                stats.overhead += leak.cs.space();

                // and peaks
                if( leak.size   > stats.leak_peak  ) stats.leak_peak = leak.size;
                if( stats.usage > stats.usage_peak ) stats.usage_peak = stats.usage;
            }

            acquired = false;
            mutex->unlock();

            return ptr;
        }
    };
}


namespace tracey
{
    void *watch( const void *ptr, size_t size ) {
        tracer( (void *)ptr, size );
        return (void *)ptr;
    }
    void *forget( const void *ptr ) {
        size_t forget = (~0);
        tracer( (void *)ptr, forget );
        return (void *)ptr;
    }
    void enable() {
        kTraceyEnabledSoft = true;
    }
    void disable() {
        kTraceyEnabledSoft = false;
    }
    void clear() {
        size_t opcode = 1, special_fn = (~0) - 1;
        tracer( &opcode, special_fn );
    }
    std::string report() {
        size_t special_fn = (~0) - 2;
        return *((std::string *)tracey::tracer( (void*)&report, special_fn ));
    }
    void view( const std::string &report ) {
        std::string copy = report;
        size_t special_fn = (~0) - 3;
        tracer( &copy, special_fn );
    }
    std::string summary() {
        size_t special_fn = (~0) - 4;
        return *((std::string *)tracey::tracer( (void*)&report, special_fn ));
    }
    void fail( const char *message ) {
        kTraceyPrintf( "%s\n", message );
        kTraceyAssert( !"<tracey/tracey.cpp> says: fail() requested" );
        $throw(
            throw std::runtime_error( message );
        )
        $telse(
            kTraceyDie(__LINE__);
        )
    }
    void badalloc() {
        $throw(
            throw std::bad_alloc();
        )
        tracey::fail( "<tracey/tracey.cpp> says: error! out of memory" );
    }
    std::string version() {
        return "tracey-" TRACEY_VERSION;  /* format is apibreak.apienhacement.fixes */
    }
    std::string url() {
        return "https://github.com/r-lyeh/tracey";
    }
    static void runtime_checks() {
        // if md mdd + wild pointers
        //
    }
    static std::string settings( const std::string &prefix ) {
        tracey::string out;
        out += tracey::string( "\1\2 ready" kTraceyCharLinefeed, prefix, tracey::version() );
        if( tracey::lookup(url) == "????" ) {
            out += tracey::string( "\1failed to decode symbols!! Is debug information available?" $msvc(" Are .PDB files available?") kTraceyCharLinefeed, prefix );
        }
        out += tracey::string( "\1using \2 as realloc" kTraceyCharLinefeed, prefix, tracey::lookup(kTraceyRealloc) );
        out += tracey::string( "\1using \2 as memset" kTraceyCharLinefeed, prefix, tracey::lookup(kTraceyMemset) );
        out += tracey::string( "\1using \2 as printf" kTraceyCharLinefeed, prefix, tracey::lookup(kTraceyPrintf) );
        out += tracey::string( "\1using \2 as exit" kTraceyCharLinefeed, prefix, tracey::lookup(kTraceyDie) );
        out += tracey::string( "\1using \2 as fopen" kTraceyCharLinefeed, prefix, tracey::lookup(kTraceyfOpen) );
        out += tracey::string( "\1using \2 as fclose" kTraceyCharLinefeed, prefix, tracey::lookup(kTraceyfClose) );
        out += tracey::string( "\1using \2 as fprintf" kTraceyCharLinefeed, prefix, tracey::lookup(kTraceyfPrintf) );
        out += tracey::string( "\1with C++ exceptions=\2" kTraceyCharLinefeed, prefix, $throw("enabled") $telse("disabled") );
        out += tracey::string( "\1with kTraceyBudgetOverhead=\2%" kTraceyCharLinefeed, prefix, (100 + kTraceyBudgetOverhead) );
        out += tracey::string( "\1with kTraceyMaxStacktraces=\2 range[\3..\4]" kTraceyCharLinefeed, prefix, int(kTraceyMaxStacktraces), int(kTraceyStacktraceSkipBegin), int(kTraceyStacktraceSkipEnd) );
        // kTraceyCharLinefeed
        // kTraceyCharTab
        out += tracey::string( "\1with kTraceyReportWildPointers=\2" kTraceyCharLinefeed, prefix, kTraceyReportWildPointers ? "yes" : "no" );
        out += tracey::string( "\1with kTraceyDefineMemoryOperators=\2" kTraceyCharLinefeed, prefix, kTraceyDefineMemoryOperators ? "yes" : "no" );
        out += tracey::string( "\1with kTraceyMemsetAllocations=\2" kTraceyCharLinefeed, prefix, kTraceyMemsetAllocations ? "yes" : "no" );
        out += tracey::string( "\1with kTraceyStacktraceSkipBegin=\2" kTraceyCharLinefeed, prefix, int(kTraceyStacktraceSkipBegin) );
        out += tracey::string( "\1with kTraceyStacktraceSkipEnd=\2" kTraceyCharLinefeed, prefix, int(kTraceyStacktraceSkipEnd) );
        out += tracey::string( "\1with kTraceyReportOnExit=\2" kTraceyCharLinefeed, prefix, kTraceyReportOnExit ? "yes" : "no" );
        out += tracey::string( "\1with kTraceyWebserverPort=\2" kTraceyCharLinefeed, prefix, int(kTraceyWebserverPort) );
        out += tracey::string( "\1with kTraceyHookLegacyCRT=\2" kTraceyCharLinefeed, prefix, int(kTraceyHookLegacyCRT) );
        out += tracey::string( "\1with kTraceyEnabled=\2" kTraceyCharLinefeed, prefix, int(kTraceyEnabled) );
        return out;
    }
    std::string settings() {
        return tracey::settings("<tracey/tracey.cpp> says: ");
    }

    // Extra

    bool nop() {
        tracey::free( tracey::forget( tracey::watch( tracey::malloc( 1 ), 1 ) ) );
        return true;
    }
    bool install_c_hooks() {
        return false;
    }

    void *realloc( void *ptr, size_t resize ) {
        static const bool init = install_c_hooks();

        ptr = kTraceyRealloc( ptr, (size_t)( resize + (kTraceyBudgetOverhead * resize) / 100.0 ) );

        if( !ptr && resize )
            tracey::badalloc();

        return ptr;
    }
    void *malloc( size_t size ) {
        void *ptr = tracey::realloc( 0, size );
        return kTraceyMemsetAllocations ? kTraceyMemset( ptr, kTraceyMemsetChar, size ) : ptr;
    }
    void *calloc( size_t num, size_t size ) {
        return kTraceyMemsetAllocations ? tracey::malloc( num * size ) : memset( tracey::malloc( num * size ), kTraceyMemsetChar, num * size );
    }
    void *free( void *ptr ) {
        return tracey::realloc( ptr, 0 ), ptr;
    }

    void *amalloc( size_t size, size_t alignment ) {
        // kids, dont do this at home. this is horrible.
        std::vector<void *> invalids( 1, tracey::malloc(size) );
        if( alignment ) {
#          define is_aligned(POINTER, BYTE_COUNT) \
                (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)
            while( !is_aligned(invalids.back(),alignment) )
                invalids.push_back( tracey::malloc( size ) );
            if( invalids.size() > 1 ) {
                std::swap( invalids.front(), invalids.back() );
                for( unsigned i = 1; i < invalids.size(); ++i ) {
                    tracey::free( invalids[i] );
                }
            }
        }
        return invalids[0];
    }
}

#if kTraceyDefineMemoryOperators

//* Custom memory operators (with no exceptions)

void *operator new( size_t size, const std::nothrow_t &t ) throw() {
    return tracey::watch( tracey::malloc( size ), size );
}

void *operator new[]( size_t size, const std::nothrow_t &t ) throw() {
    return tracey::watch( tracey::malloc( size ), size );
}

void operator delete( void *ptr, const std::nothrow_t &t ) throw() {
    tracey::free( tracey::forget( ptr ) );
}

void operator delete[]( void *ptr, const std::nothrow_t &t ) throw() {
    tracey::free( tracey::forget( ptr ) );
}

//* Custom memory operators (with exceptions)

void *operator new( size_t size ) throw(std::bad_alloc) {
    return tracey::watch( tracey::malloc( size ), size );
}

void *operator new[]( size_t size ) throw(std::bad_alloc) {
    return tracey::watch( tracey::malloc( size ), size );
}

void operator delete( void *ptr ) throw() {
    tracey::free( tracey::forget( ptr ) );
}

void operator delete[]( void *ptr ) throw() {
    tracey::free( tracey::forget( ptr ) );
}

#endif

namespace tracey {

#if kTraceyWebserverPort

static std::string p( const std::string &str ) { return tracey::string("<p>\1</p>", str); }
static std::string h1( const std::string &str ) { return tracey::string("<h1>\1</h1>", str); }
static std::string pre( const std::string &str ) { return tracey::string("<xmp>\1</xmp>", str); }
static std::string html( const std::string &str ) { return tracey::string("<html>\1</html>", str); }
static std::string style( const std::string &str ) { return tracey::string("<style>\1</style>", str); }
static std::string body( const std::string &str ) { return tracey::string("<body>\1</body>", str); }
static std::string a( const std::string &text, const std::string &href = "#" ) { return tracey::string("<a href=\"\1\">\2</a>", href, text); }
static std::string ul( const std::string &str ) { return tracey::string("<ul>\1</ul>", str ); }
static std::string li( const std::string &str ) { return tracey::string("<li>\1</li>", str ); }

static tracey::string get_html_template() {
    return $quote(
        <!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
        <html>
        <head>
            <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
            <title>{TITLE}</title>
        </head>
        <body>
            <div id="header">
                <h2>{TITLE}</h2>
            </div>
            <div id="content">
                <p>{SUMMARY}</p>
                <p>{REPORT}</p>
                <p>{SETTINGS}</p>
            </div>
        </body>
        </html>
    );
}

static void webmain( void *arg ) {
    if( kTraceyWebserverPort ) {

        struct local {
            static 
            int GET_root( route66::request &req, std::ostream &headers, std::ostream &content ) {
                headers << route66::mime(".html");
                content << get_html_template().
                    replace("{TITLE}", "tracey webserver").
                    replace("{SETTINGS}", pre( tracey::settings("") ) ).
                    replace("{REPORT}", a("generate leak report (may take a while)", "report")).
                    replace("{SUMMARY}",  tracey::summary() );
                return 200;
            };
            static
            int GET_report( route66::request &req, std::ostream &headers, std::ostream &content ) {
                headers << route66::mime(".html");
                content << p("Tracey generating report");
                tracey::view( tracey::report() );
                return 200;
            } 
        };

        route66::create( kTraceyWebserverPort, "GET /", local::GET_root );
        route66::create( kTraceyWebserverPort, "GET /report", local::GET_report );
    }
}

#else

static void webmain( void *arg )
{}

#endif

} // tracey::

namespace tracey {
    #pragma comment(lib, "user32.lib")
    static void hotkeymain( void *arg ) {
        volatile bool &sitdown = *(volatile bool *)arg;
        sitdown = true;

        $windows(
            for(;;) {
                if( GetAsyncKeyState(VK_NUMLOCK) ) {
                    tracey::view( tracey::report() );
                }
                Sleep( 1000/60 );
            }
        )
    }
}

// platform related, externals here.

#ifdef INCLUDE_GOOGLEINL_AT_END
#undef INCLUDE_GOOGLEINL_AT_END
#include "google.hpp"
#include "google.inl"
#ifdef _MSC_VER
/* #pragma init_seg( ".CRT$XCA" ) */
#endif
static const bool lazy_init = (tracey::init(), tracey::install_c_hooks());
#endif

// C++ runtime API extension
namespace tracey {
    scope::scope() {
        tracey::clear();
        tracey::enable();
    }
    scope::~scope() {
        tracey::disable();
        if( tracey::stats.num_leaks > 0 ) tracey::view( tracey::report() );
    }
}

// C runtime API 
extern "C" {
        // Basic API
        void *tracey_watch( const void *ptr, size_t size ) {
            return tracey::watch( ptr, size );
        }
        void *tracey_forget( const void *ptr ) {
            return tracey::forget( ptr );
        }
        void  tracey_enable() {
            tracey::enable();
        }
        void  tracey_disable() {
            tracey::disable();
        }
        void  tracey_clear() {
            return tracey::clear();
        }

        // Information API  /*[!] free() return value after use */
        char *tracey_report() {
            return strdup( tracey::report().c_str() );
        }
        char *tracey_summary() {
            return strdup( tracey::summary().c_str() );
        }   
        char *tracey_version() {
            return strdup( tracey::version().c_str() );
        }
        char *tracey_url() {
            return strdup( tracey::url().c_str() );
        }
        char *tracey_settings() {
            return strdup( tracey::settings().c_str() );
        }

        // Report API
        void  tracey_view( const char *const report ) {
            tracey::view( report );
        }
        void  tracey_view_report() {
            tracey::view( tracey::report() );
        }

        // Unchecked memory API
        void *tracey_unchecked_amalloc( size_t size, size_t alignment ) {
            return tracey::amalloc( size, alignment );
        }
        void *tracey_unchecked_calloc( size_t num, size_t size ) {
            return tracey::calloc( num, size );
        }
        void *tracey_unchecked_malloc( size_t size ) {
            return tracey::malloc( size );
        }
        void *tracey_unchecked_realloc( void *ptr, size_t resize ) {
            return tracey::realloc( ptr, resize );
        }
        void  tracey_unchecked_free( void *ptr ) {
            tracey::free( ptr );
        }

        // Checked memory API
        void *tracey_checked_amalloc( size_t size, size_t alignment ) {
            return tracey::watch( tracey::amalloc( size, alignment ), size );
        }
        void *tracey_checked_calloc( size_t num, size_t size ) {
            return tracey::watch( tracey::calloc( num, size ), num * size );
        }
        void *tracey_checked_malloc( size_t size ) {
            return tracey::watch( tracey::malloc( size ), size );
        }
        void *tracey_checked_realloc( void *ptr, size_t resize ) {
            return tracey::watch( tracey::realloc( tracey::forget( ptr ), resize ), resize );
        }
        void  tracey_checked_free( void *ptr ) {
            tracey::free( tracey::forget( ptr ) );
        }

        // Crash API
        void tracey_fail( const char *message ) {
            tracey::fail( message );
        }
        void tracey_badalloc() {
            tracey::badalloc();
        }
}
