///* Tracey, a simple and lightweight memory leak detector
// * Copyright (c) 2011,2012,2013,2014 r-lyeh
// * ZLIB/LibPNG licensed.

// * Features:
// * - tiny
// * - clean
// * - callstack based
// * - lazy. should work with no source modification
// * - macroless API. no new/delete macros
// * - embeddable. just link it against your project
// * - C hooks (optional) (win32 only)
// * - C++ hooks (optional)
// * - web server (optional)

// * To do:
// * - support for C hooks on non-win archs:
// *   - http://stackoverflow.com/questions/262439/create-a-wrapper-function-for-malloc-and-free-in-c
// *   - http://src.chromium.org/svn/trunk/src/tools/memory_watcher/memory_hook.cc
// *   - void __f () { } // Do something.
// *     void f () __attribute__ ((weak, alias ("__f")));
// *   - or http://man7.org/linux/man-pages/man3/malloc_hook.3.html
// *   - as seen in https://github.com/emeryberger/Heap-Layers/blob/master/wrappers/gnuwrapper.cpp
// * - memory manager checks
// *   - Read before allocating memory
// *   - Write before allocating memory
// *   - Read before beginning of allocation
// *   - Write before beginning of allocation
// *   - Read after end of allocation
// *   - Write after end of allocation
// *   - Read after deallocation
// *   - Write after deallocation
// *   - Failure to deallocate memory
// *   - Deallocating memory twice
// *   - Deallocating nonallocated memory
// *   - Zero-size memory allocation
// * - optional pools for microallocations
// * - reports per:
// *   - symbol: namespace/class/function
// *   - author: blame, candidates (caller and callee for highest apparitions founds on blame(symbol(ns/cl/fn)))
// *   - location: filename, directory
// *   - origin: module/thread
// *   - wasted size, % total, hits
// * - ignores per project/session:
// *   - symbol: namespace/class/function
// *   - author: blame, candidates
// *   - location: filename, directory
// *   - origin: module/thread
// * - versioning support (svn/git...)
// * - cloud account linking support (github/gitorius/etc):
// *   - auto commit/pull symbol maps
// *   - auto commit/pull settings
// *   - auto commit reports
// * - auto updates on ui: load webserver templates from official github repo

// * References:
// * - http://www.codeproject.com/KB/cpp/allocator.aspx
// * - http://drdobbs.com/cpp/184403759

// * - rlyeh ~~ listening to Long Distance Calling / Metulsky Curse Revisited
// */

// A few speed tweaks before loading STL on MSVC

#ifdef _SECURE_SCL
#undef _SECURE_SCL
#endif
#define _SECURE_SCL 0
#ifdef _HAS_ITERATOR_DEBUGGING
#undef _HAS_ITERATOR_DEBUGGING
#endif
//#define _HAS_ITERATOR_DEBUGGING 0

#if !(defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#define NDEBUG
#endif
#if !(defined(DEBUG) || defined(_DEBUG)) && !defined(_NDEBUG)
#define _NDEBUG
#endif

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
#include "tracey.hpp"
// }

// external; tree implementation {

//#line 1 "oak.hpp"
// a simple tree container, zlib/libpng licensed.
// - rlyeh ~~ listening to Buckethead - The Moltrail #2

//#pragma once
#include <cassert>
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#define OAK_VERSION "1.0.0" // (2015/10/25) Semantic versioning adherence; fix csv() with non-string keys

namespace oak
{
    // config
#   ifndef OAK_VERBOSE
        enum { OAK_VERBOSE = false };
#   endif

    // tree class
    // [] means read-writeable (so is <<)
    // () means read-only access

    template< typename K, typename V = int, typename P = std::less< K > >
    class tree : public std::map< K, tree<K,V,P>, P > {

            typedef typename std::map< K, tree<K,V,P>, P > map;

            template< typename T >
            T zero() const {
                return std::pair<T,T>().first;
            }

            template<typename T>
            T &invalid() const {
                static T t;
                return t = T(), t;
            }

            V value;
            tree *parent;

        public:

            tree() : map(), value(zero<V>()) {
                parent = this;
            }

            tree( const tree &t ) : map(), value(zero<V>()) {
                parent = this;
                operator=(t);
            }

            // tree clone

            tree &operator=( const tree &t ) {
                if( this != &t ) {
                    this->clear();
                    get() = zero<V>();
                    operator+=(t);
                }
                return *this;
            }

            // tree merge

            tree &operator+=( const tree &t ) {
                if( this != &t ) {
                    for( typename tree::const_iterator it = t.begin(), end = t.end(); it != end; ++it ) {
                        this->map::insert( *it );
                    }
                    get() = t.get();
                }
                return *this;
            }

            // tree search ; const safe find: no insertions on new searches

            const tree &at( const K &t ) const {
                typename map::const_iterator find = this->find( t );
                return find != this->end() ? find->second : invalid<tree>();
            }

            // tree insertion

            tree &insert( const K &t ) {
                map &children = *this;
                ( children[t] = children[t] ).parent = this;
                return children[t];
            }
            tree &erase( const K &t ) {
                typename map::iterator find = this->find(t);
                if( find != this->end() ) this->map::erase(t);
                return *this;
            }

            // recursive values

            V &get() {
                return value;
            }
            const V &get() const {
                return value;
            }

            template<typename other>
            tree &set( const other &t ) {
                get() = t;
                return *this;
            }
            template<typename other>
            tree &setup( const other &t ) {
                if( !is_root() ) {
                    up().set(t).setup(t);
                }
                return *this;
            }
            template<typename other>
            tree &setdown( const other &t ) {
                for( typename tree::iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                    it->second.set(t).setdown( t );
                }
                return *this;
            }

            V getdown() const {
                V value = get();
                for( typename tree::const_iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                    value += it->second.getdown();
                }
                return value;
            }

            // sugars

            tree &clone( const tree &t ) {
                return operator=( t );
            }
            tree &assign( const tree &t ) {
                return operator=( t );
            }

            template<typename other>
            tree &operator=( const other &t ) {
                return set(t);
            }
            template<typename other>
            tree &operator+=( const other &t ) {
                return get() += t, *this;
            }

            tree &merge( const tree &t ) {
                return operator +=( t );
            }
            tree &operator[]( const K &t ) {
                return insert( t );
            }
            const tree &operator()( const K &t ) const {
                return at( t );
            }

            bool empty( const K &t ) const { // @todo: subempty
                return this->find(t) == this->end();
            }
            bool has( const K &t ) const {
                return !empty( t );
            }

            bool is_valid() const {
                return this != &invalid<tree>();
            }
            bool operator!() const {
                return !is_valid();
            }

            const map &children() const {
                return *this;
            }
            map &children() {
                return *this;
            }

            tree &up() {
                return *parent;
            }
            const tree &up() const {
                return *parent;
            }

            bool is_root() const {
                return parent == this;
            }
            const tree &root() const {
                if( !is_root() ) return parent->root();
                return *this;
            }
            tree &root() {
                if( !is_root() ) return parent->root();
                return *this;
            }

            // tools

            template<typename ostream>
            void csv( ostream &cout = std::cout, const std::string &prefix = std::string(), unsigned depth = 0 ) const {
                for( typename tree::const_iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                    cout << prefix << "/" << it->first << "," << it->second.get() << std::endl;
                    std::stringstream ss;
                    ss << prefix << "/" << it->first;
                    it->second.csv( cout, ss.str(), depth + 1 );
                }
            }

            std::string as_csv() const {
                std::stringstream ss;
                return csv( ss ), ss.str();
            }

            template<typename ostream>
            void print( ostream &cout = std::cout, unsigned depth = 0 ) const {
                std::string tabs( depth, '\t' );
                for( typename tree::const_iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                    cout << tabs << "[" << this->size() << "] " << it->first << " (" << it->second.get() << ")";
                    if( !OAK_VERBOSE ) cout << std::endl;
                    else               cout << ".t=" << this << ",.p=" << parent << std::endl;
                    it->second.print( cout, depth + 1 );
                }
            }

            template<typename U, typename ostream>
            void print( const std::map< K, U > &tmap, ostream &cout, unsigned depth = 0 ) const {
                std::string tabs( depth, '\t' );
                for( typename tree::const_iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                    cout << tabs << "[" << this->size() << "] " << tmap.find( it->first )->second << " (" << it->second.get() << ")";
                    if( !OAK_VERBOSE ) cout << std::endl;
                    else               cout << ".t=" << this << ",.p=" << parent << std::endl;
                    it->second.print( tmap, cout, depth + 1 );
                }
            }

            template<typename ostream>
            inline friend ostream &operator<<( ostream &os, const tree &self ) {
                return self.print( os ), os;
            }

            template<typename U>
            tree<U> rekey( const std::map< K, U > &map ) const {
                tree<U> utree;
                for( typename tree::const_iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                    typename std::map< K, U >::const_iterator find = map.find( it->first );
                    assert( find != map.end() );
                    utree[ find->second ] += it->second.rekey( map );
                    utree[ find->second ].get() = it->second.get();
                }
                return utree;
            }
            template<typename U>
            tree<U> rekey( const std::map< U, K > &map ) const {
                // this could be faster
                tree<U> utree;
                for( typename std::map< U, K >::const_iterator it = map.begin(), end = map.end(); it != end; ++it ) {
                    typename tree::const_iterator find = this->find( it->second );
                    if( find == this->end() ) continue;
                    utree[ it->first ] += find->second.rekey( map );
                    utree[ it->first ].get() = find->second.get();
                }
                return utree;
            }

            tree collapse() const {
                tree t;
                if( this->size() == 1 ) {
                    return this->begin()->second.collapse();
                } else {
                    for( typename tree::const_iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                        t[ it->first ] += it->second.collapse();
                        t[ it->first ].get() = it->second.get();
                    }
                }
                return t;
            }

            V refresh() {
                V value = !this->size() ? get() : zero<V>();
                for( typename tree::iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                    value += it->second.refresh();
                }
                return get() = value;
            }

            // c++03 case, user defined operator()
            // c++11 case, accept lambdas as arguments
            template<typename T>
            tree &walk( const T &predicate ) {
                for( typename tree::iterator it = this->begin(), it_next = it; it != this->end(); it = it_next) {
                    ++it_next;
                    if( predicate(*this, it) ) {
                        it->second.walk( predicate );
                    }
                }
                return *this;
            }

            // alias
            template<typename T>
            tree &walk() {
                return walk( T() );
            }
    };
}


// }

// external; macros, OS utils. Here is where the fun starts {
#   define HEAL_MAX_TRACES kTraceyMaxStacktraces
#   define heal tracey_heal

//#line 1 "heal.cpp"
// Heal is a lightweight C++ framework to aid and debug applications.
// - rlyeh, zlib/libpng licensed // ~listening to Kalas - Monuments to Ruins

// Callstack code is based on code by Magnus Norddahl (See http://goo.gl/LM5JB)
// Mem/CPU OS code is based on code by David Robert Nadeau (See http://goo.gl/8P5Jqv)
// Distributed under Creative Commons Attribution 3.0 Unported License
// http://creativecommons.org/licenses/by/3.0/deed.en_US

// A few tweaks before loading STL on MSVC
// This improves stack unwinding.

#ifdef _SECURE_SCL
#undef _SECURE_SCL
#endif
#define _SECURE_SCL 0
#ifdef _ITERATOR_DEBUG_LEVEL
#undef _ITERATOR_DEBUG_LEVEL
#define _ITERATOR_DEBUG_LEVEL 1
#endif
//#define _HAS_ITERATOR_DEBUGGING 0

// Standard headers

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

// System headers

#ifdef _WIN32
//#   define UNICODE
//#   define _UNICODE
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   include <windows.h>
#   include <commctrl.h>
#   include <string.h>
#   pragma comment(lib, "comctl32.lib")
#   if defined _M_IX86
#       pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   elif defined _M_IA64
#       pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   elif defined _M_X64
#       pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   else
#       pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   endif
#   // unwinding
#   if defined(DEBUG) || defined(_DEBUG)
#       include <CrtDbg.h>
#   endif
#   pragma warning( push )
#   pragma warning( disable : 4091 )
#   include <DbgHelp.h>
#   pragma warning( pop )
#   pragma comment(lib, "dbghelp.lib")
#   ifndef TD_SHIELD_ICON
#       define TD_SHIELD_ICON          MAKEINTRESOURCEW(-4)
#   endif
#   define backtrace(a,b) 0
#   define backtrace_symbols(a,b) 0
#else
#   include <unistd.h>
#   include <signal.h>
#   include <sys/time.h>
#   include <sys/types.h>
//  --
#   if defined(HAVE_SYS_SYSCTL_H) && \
        !defined(_SC_NPROCESSORS_ONLN) && !defined(_SC_NPROC_ONLN)
#       include <sys/param.h>
#       include <sys/sysctl.h>
#   endif
//  --
#   include <execinfo.h>
//  --
#   include <cxxabi.h>

#endif

#ifdef __MINGW32__
#define backtrace(a,b) 0
#define backtrace_symbols(a,b) 0
#endif

// API


//#line 1 "heal.hpp"
// -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< --

// std 11 <-> 03/w-boost bridge compatiblity layer, plus a few macro utils.
// - rlyeh, zlib/libpng licensed.

#ifndef __BRIDGE_HPP__
#define __BRIDGE_HPP__

#ifdef __SSE__
#   define BOOST_HAS_INT128 1
#   include <xmmintrin.h>
#endif

#if (__cplusplus < 201103L && !defined(_MSC_VER)) || (defined(_MSC_VER) && (_MSC_VER < 1700)) || (defined(__GLIBCXX__) && __GLIBCXX__ < 20130322L)
#   define BRIDGE_VERSION 2003
#   include <boost/functional.hpp> // if old libstdc++ or msc libs are found, use boost::function
#   include <boost/function.hpp>   //
#   include <boost/thread.hpp>     // and boost::thread
#   include <boost/cstdint.hpp>
#   include <boost/type_traits.hpp>
#   include <boost/bind.hpp>
#   include <boost/bind/placeholders.hpp>
namespace std {
    namespace placeholders {
        //...
    }
    using namespace boost;
}
#else
#   define BRIDGE_VERSION 2011
#   include <functional>       // else assume modern c++11 and use std::function<> instead
#   include <mutex>            // and std::mutex
#   include <thread>           // and std::thread
#   include <cstdint>
#endif

#if BRIDGE_VERSION >= 2011
#define $cpp11          $yes
#define $cpp03          $no
#else
#define $cpp11          $no
#define $cpp03          $yes
#endif

// Thread Local Storage

#if defined(__MINGW32__) || defined(__SUNPRO_C) || defined(__xlc__) || defined(__GNUC__) || defined(__clang__) || defined(__GNUC__) // __INTEL_COMPILER on linux
//   MingW, Solaris Studio C/C++, IBM XL C/C++,[3] GNU C,[4] Clang[5] and Intel C++ Compiler (Linux systems)
#    define $tls(x) __thread x
#else
//   Visual C++,[7] Intel C/C++ (Windows systems),[8] C++Builder, and Digital Mars C++
#    define $tls(x) __declspec(thread) x
#endif

// OS utils. Here is where the fun starts... good luck

#define $quote(...)     #__VA_ARGS__
#define $comment(...)   $no
#define $uncomment(...) $yes

#define $yes(...)     __VA_ARGS__
#define $no(...)

#define $on(v)        (0 v(+1))  // usage: #if $on($msvc)
#define $is           $on        // usage: #if $is($debug)
#define $has(...)     $clang(__has_feature(__VA_ARGS__)) $celse(__VA_ARGS__) // usage: #if $has(cxx_exceptions)

#if defined(_WIN32)
#   define $windows   $yes
#   define $welse     $no
#else
#   define $windows   $no
#   define $welse     $yes
#endif

#ifdef __APPLE__
#   define $apple     $yes
#   define $aelse     $no
#else
#   define $apple     $no
#   define $aelse     $yes
#endif

#ifdef __linux__
#   define $linux     $yes
#   define $lelse     $no
#else
#   define $linux     $no
#   define $lelse     $yes
#endif

// Compiler utils

#if ULONG_MAX == 4294967295
#   define $bits64    $yes
#   define $bits32    $no
#else
#   define $bits64    $no
#   define $bits32    $yes
#endif

#if defined(NDEBUG) || defined(_NDEBUG) || defined(RELEASE)
#   define $release   $yes
#   define $debug     $no
#else
#   define $release   $no
#   define $debug     $yes
#endif

#if defined(NDEVEL) || defined(_NDEVEL) || defined(PUBLIC)
#   define $public    $yes
#   define $devel     $no
#else
#   define $public    $no
#   define $devel     $yes
#endif

#if defined(__GNUC__) || defined(__MINGW32__)
#   define $gnuc      $yes
#   define $gelse     $no
#else
#   define $gnuc      $no
#   define $gelse     $yes
#endif

#ifdef _MSC_VER
#   define $msvc      $yes
#   define $melse     $no
#else
#   define $msvc      $no
#   define $melse     $yes
#endif

#ifdef __clang__
#   define $clang     $yes
#   define $celse     $no
#else
#   define $clang     $no
#   define $celse     $yes
#endif

#if $on($msvc) || $on($gnuc) || $on($clang)
#   define $undefined_compiler $no
#else
#   define $undefined_compiler $yes
#endif

#if $on($windows) || $on($linux) || $on($apple)
#   define $undefined_os $no
#else
#   define $undefined_os $yes
#endif

#if $on($gnuc) || $on($clang)
#   define $likely(expr)    (__builtin_expect(!!(expr), 1))
#   define $unlikely(expr)  (__builtin_expect(!!(expr), 0))
#else
#   define $likely(expr)    ((expr))
#   define $unlikely(expr)  ((expr))
#endif

// try to detect if exceptions are enabled...

#if (defined(_HAS_EXCEPTIONS) && (_HAS_EXCEPTIONS > 0)) || \
    (defined(_STLP_USE_EXCEPTIONS) && (_STLP_USE_EXCEPTIONS > 0)) || \
    (defined(HAVE_EXCEPTIONS)) || \
    (defined(__EXCEPTIONS)) || \
    (defined(_CPPUNWIND)) || \
    ($has(cxx_exceptions)) /*(__has_feature(cxx_exceptions))*/
#   define $throw     $yes
#   define $telse     $no
#else
#   define $throw     $no
#   define $telse     $yes
#endif

// create a $warning(...) macro
// usage: $warning("this is shown at compile time")
#if $on($msvc)
#   define $warning(msg) __pragma( message( msg ) )
#elif $on($gnuc) || $on($clang)
#   define $$warning$impl(msg) _Pragma(#msg)
#   define $warning(msg) $$warning$impl( message( msg ) )
#else
#   define $warning(msg)
#endif

// create a $warning(...) macro
// usage: $warning("this is shown at compile time")
#define $$todo$stringize$impl(X) #X
#define $$todo$stringize(X) $$todo$stringize$impl(X)
#define $todo(...) $warning( __FILE__ "(" $$todo$stringize(__LINE__)") : $todo - " #__VA_ARGS__ " - [ "__func__ " ]" )

#endif // __BRIDGE_HPP__

// -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< -- 8< --

// Heal is a lightweight C++ framework to aid and debug applications.
// - rlyeh, zlib/libpng licensed // ~listening to Kalas - Monuments to Ruins

// Callstack code is based on code by Magnus Norddahl (See http://goo.gl/LM5JB)
// Mem/CPU OS code is based on code by David Robert Nadeau (See http://goo.gl/8P5Jqv)
// Distributed under Creative Commons Attribution 3.0 Unported License
// http://creativecommons.org/licenses/by/3.0/deed.en_US

#ifndef __HEALHPP__
#define __HEALHPP__

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include <iostream>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <deque>

#define HEAL_VERSION "1.0.0" // (2015/10/01) Semantic versioning adherence and clean ups

/* public API */

namespace heal {

    typedef std::function< int( const std::string &in ) > heal_callback_in;

    extern std::vector< heal_callback_in > warns;
    extern std::vector< heal_callback_in > fails;

    void warn( const std::string &error );
    void fail( const std::string &error );

    void add_worker( heal_callback_in fn );

    void die( const std::string &reason, int errorcode = -1 );
    void die( int errorcode = -1, const std::string &reason = std::string() );

    void breakpoint();
    bool debugger( const std::string &reason = std::string() );

    void alert();
    void alert( const          char *t, const std::string &title = std::string() );
    void alert( const   std::string &t, const std::string &title = std::string() );
    void alert( const std::istream &is, const std::string &title = std::string() );
    void alert( const        size_t &t, const std::string &title = std::string() );
    void alert( const        double &t, const std::string &title = std::string() );
    void alert( const         float &t, const std::string &title = std::string() );
    void alert( const           int &t, const std::string &title = std::string() );
    void alert( const          char &t, const std::string &title = std::string() );
    void alert( const          bool &t, const std::string &title = std::string() );
    void errorbox( const std::string &body = std::string(), const std::string &title = std::string() );
    std::string prompt( const std::string &current_value = std::string(), const std::string &title = std::string(), const std::string &caption = std::string() );

    bool is_debug();
    bool is_release();
    bool is_asserting();

    bool is_devel();
    bool is_public();

    #ifndef HEAL_MAX_TRACES
    #define HEAL_MAX_TRACES 128
    #endif

    struct callstack /* : public std::vector<const void*> */ {
        enum { max_frames = HEAL_MAX_TRACES };
        std::vector<void *> frames;
        callstack( bool autosave = false );
        size_t space() const;
        void save( unsigned frames_to_skip = 0 );
        std::vector<std::string> unwind( std::size_t from = 0, std::size_t to = ~0 ) const;
        std::vector<std::string> str( const char *format12 = "#\1 \2\n", size_t skip_begin = 0 ) const;
        std::string flat( const char *format12 = "#\1 \2\n", size_t skip_begin = 0 ) const;
    };

    template<typename T>
    static inline
    std::string lookup( T *ptr ) {
        callstack cs;
        cs.frames.push_back( (void *)ptr );
        std::vector<std::string> stacktrace = cs.unwind();
        return stacktrace.size() ? stacktrace[0] : std::string("????");
    }

    std::string demangle( const std::string &mangled );
    std::vector<std::string> stacktrace( const char *format12 = "#\1 \2\n", size_t skip_initial = 0 );
    std::string stackstring( const char *format12 = "#\1 \2\n", size_t skip_initial = 0 );

    std::string hexdump( const void *data, size_t num_bytes, const void *self = 0 );

    template<typename T> inline std::string hexdump( const T& obj ) {
        return hexdump( obj.data(), obj.size() * sizeof(*obj.begin()), &obj );
    }
    $cpp11(
    template<> inline std::string hexdump( const std::nullptr_t &obj ) {
        return hexdump( 0,0,0 );
    }
    )
    template<> inline std::string hexdump( const char &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const short &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const long &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const long long &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const unsigned char &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const unsigned short &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const unsigned long &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const unsigned long long &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const float &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const double &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }
    template<> inline std::string hexdump( const long double &obj ) {
        return hexdump( &obj, sizeof(obj), &obj );
    }/*
    template<size_t N> inline std::string hexdump( const char (&obj)[N] ) {
        return hexdump( &obj, sizeof(char) * N, &obj );
    }*/

    template<typename T> inline std::string hexdump( const T* obj ) {
        if( !obj ) return hexdump(0,0,0);
        return hexdump( *obj );
    }
    template<> inline std::string hexdump( const char *obj ) {
        if( !obj ) return hexdump(0,0,0);
        return hexdump( std::string(obj) );
    }

    std::string timestamp();
}

#endif // __HEALHPP__

namespace heal {
    // sfstring is a safe string replacement that does not rely on stringstream
    // this is actually safer on corner cases, like crashes, exception unwinding and in exit conditions
    class sfstring : public std::string
    {
        public:

        // basic constructors

        sfstring() : std::string()
        {}

        template<size_t N>
        sfstring( const char (&cstr)[N] ) : std::string( cstr )
        {}

        // constructor sugars

#if 0
        // version that may crash on /MT on destructors
        // (just because it depends on std::locale which may be deinitialized before crashing code)
        template <typename T>
        /* explicit */ sfstring( const T &t ) : std::string()
        {
            std::stringstream ss;
            ss.precision( std::numeric_limits< long double >::digits10 + 1 );
            if( ss << t )
                this->assign( ss.str() );
        }
#else
        template<typename T>
        sfstring( const T &t ) : std::string( std::string(t) )
        {}
        sfstring( const std::string &t ) : std::string( t )
        {}

        sfstring( const int &t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%d", t ) > 0 ) this->assign(buf);
        }
        sfstring( const uint16_t &t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%" SCNu16, t ) > 0 ) this->assign(buf);
        }
        sfstring( const uint32_t &t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%" SCNu32, t ) > 0 ) this->assign(buf);
        }
        sfstring( const uint64_t &t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%" SCNu64, t ) > 0 ) this->assign(buf);
        }
        sfstring( const float &t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%f", t ) > 0 ) this->assign(buf);
        }
        sfstring( const double &t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%lf", t ) > 0 ) this->assign(buf);
        }
#if $on($msvc)
        sfstring( const DWORD &t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%" SCNu32, t ) > 0 ) this->assign(buf);
        }
#elif $on($gnuc)
        sfstring( const long unsigned int &t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%" SCNu32, t ) > 0 ) this->assign(buf);
        }

#endif

        sfstring( char *t ) : std::string( t ? t : "" )
        {}
        sfstring( const char *t ) : std::string( t ? t : "" )
        {}

        sfstring( void *t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%p", t ) > 0 ) this->assign(buf);
        }
        sfstring( const void *t ) : std::string() {
            char buf[128];
            if( sprintf(buf, "%p", t ) > 0 ) this->assign(buf);
        }
#endif

        // extended constructors; safe formatting

        private:
        template<unsigned N>
        std::string &safefmt( const std::string &fmt, std::string (&t)[N] ) {
            for( std::string::const_iterator it = fmt.begin(), end = fmt.end(); it != end; ++it ) {
                unsigned index(*it);
                if( index <= N ) t[0] += t[index];
                else t[0] += *it;
            }
            return t[0];
        }
        public:

        template< typename T1 >
        sfstring( const std::string &fmt, const T1 &t1 ) : std::string() {
            std::string t[] = { std::string(), sfstring(t1) };
            assign( safefmt( fmt, t ) );
        }

        template< typename T1, typename T2 >
        sfstring( const std::string &fmt, const T1 &t1, const T2 &t2 ) : std::string() {
            std::string t[] = { std::string(), sfstring(t1), sfstring(t2) };
            assign( safefmt( fmt, t ) );
        }

        template< typename T1, typename T2, typename T3 >
        sfstring( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3 ) : std::string() {
            std::string t[] = { std::string(), sfstring(t1), sfstring(t2), sfstring(t3) };
            assign( safefmt( fmt, t ) );
        }

        template< typename T1, typename T2, typename T3, typename T4 >
        sfstring( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4 ) : std::string() {
            std::string t[] = { std::string(), sfstring(t1), sfstring(t2), sfstring(t3), sfstring(t4) };
            assign( safefmt( fmt, t ) );
        }

        template< typename T1, typename T2, typename T3, typename T4, typename T5 >
        sfstring( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5 ) : std::string() {
            std::string t[] = { std::string(), sfstring(t1), sfstring(t2), sfstring(t3), sfstring(t4), sfstring(t5) };
            assign( safefmt( fmt, t ) );
        }

        template< typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
        sfstring( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6 ) : std::string() {
            std::string t[] = { std::string(), sfstring(t1), sfstring(t2), sfstring(t3), sfstring(t4), sfstring(t5), sfstring(t6) };
            assign( safefmt( fmt, t ) );
        }

        template< typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
        sfstring( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7 ) : std::string() {
            std::string t[] = { std::string(), sfstring(t1), sfstring(t2), sfstring(t3), sfstring(t4), sfstring(t5), sfstring(t6), sfstring(t7) };
            assign( safefmt( fmt, t ) );
        }

        // chaining operators

        template< typename T >
        sfstring &operator +=( const T &t ) {
            return append( sfstring(t) ), *this;
        }

        template< typename T >
        sfstring &operator <<( const T &t ) {
            return append( sfstring(t) ), *this;
        }

        sfstring &operator <<( std::ostream &( *pf )(std::ostream &) ) {
            return *pf == static_cast<std::ostream& ( * )(std::ostream&)>( std::endl ) ? (*this) += "\n", *this : *this;
        }

        // assignment sugars

        template< typename T >
        sfstring& operator=( const T &t ) {
            if( &t != this ) {
                *this = sfstring(t);
            }
            return *this;
        }

        sfstring &operator=( const char *t ) {
            return assign( t ? t : "" ), *this;
        }

        std::string str() const {
            return *this;
        }

        size_t count( const std::string &substr ) const {
            size_t n = 0;
            std::string::size_type pos = 0;
            while( (pos = this->find( substr, pos )) != std::string::npos ) {
                n++;
                pos += substr.size();
            }
            return n;
        }

        sfstring replace( const std::string &target, const std::string &replacement ) const {
            size_t found = 0;
            sfstring s = *this;
            while( ( found = s.find( target, found ) ) != std::string::npos ) {
                s.std::string::replace( found, target.length(), replacement );
                found += replacement.length();
            }
            return s;
        }
    };

    class sfstrings : public std::deque< sfstring >
    {
        public:

        sfstrings( unsigned size = 0 ) : std::deque< sfstring >( size )
        {}

        template <typename CONTAINER>
        sfstrings( const CONTAINER &c ) : std::deque< sfstring >( c.begin(), c.end() )
        {}

        template <typename CONTAINER>
        sfstrings &operator =( const CONTAINER &c ) {
            if( &c != this ) {
                *this = sfstrings( c );
            }
            return *this;
        }

        sfstring str( const char *format1 = "\1\n" ) const {
            if( this->size() == 1 )
                return *this->begin();

            sfstring out;

            for( const_iterator it = this->begin(); it != this->end(); ++it )
                out += sfstring( format1, (*it) );

            return out;
        }

        sfstring flat() const {
            return str( "\1" );
        }
    };
}

// INFO MESSAGES
// Reminders for retrieving symbols
/*
#  if $on($msvc)
    $warning( "<heal/heal.cpp> says: do not forget /Zi, /Z7 or /C7 compiler settings! /Oy- also helps!" )
#elif $on($clang)
    $warning( "<heal/heal.cpp> says: do not forget -g compiler setting!" )
#elif $on($gnuc)
    $warning( "<heal/heal.cpp> says: do not forget -g -lpthread compiler settings!" )
#endif
*/
// ASSERT

namespace heal {

std::vector< heal_callback_in > warns(1);
std::vector< heal_callback_in > fails(1);

namespace {
    bool default_warn( const std::string &text ) {
        if( text.size() ) {
            alert( text, "Warning" );
        }
        return true;
    }
    bool default_fail( const std::string &text ) {
        if( text.size() ) {
            errorbox( text, "Error" );
        }
        if( !debugger() ) {
            alert( "Could not launch debugger" );
        }
        return true;
    }
    const bool init_warns = (warns[0] = default_warn, true);
    const bool init_fails = (fails[0] = default_fail, true);
}

void warn( const std::string &error ) {
    static bool recursive = false;
    if( !recursive ) {
        recursive = true;
        for( std::size_t i = warns.size(); i--; ) {
            if( warns[i] ) if( warns[i]( error ) ) break;
        }
        recursive = false;
    }
}

void fail( const std::string &error ) {
    static bool recursive = false;
    if( !recursive ) {
        recursive = true;
        for( std::size_t i = fails.size(); i--; ) {
            if( fails[i] ) if( fails[i]( error ) ) break;
        }
        recursive = false;
    }
}

bool is_asserting() {
    bool asserting = false;
    assert( asserting |= true );
    return asserting;
}

// IS_DEBUG
// IS_RELEASE

bool is_debug() {
    return $debug(true) $release(false);
}
bool is_release() {
    return !is_debug();
}

bool is_devel() {
    return $devel(true) $public(false);
}
bool is_public() {
    return !is_devel();
}

// DEBUGGER

#if $on($linux) || $on($apple)

    // enable core dumps for debug builds
    // after a crash try to do something like 'gdb ./a.out core'
#   if defined(NDEBUG) || defined(_NDEBUG)
        const bool are_coredumps_enabled = false;
#   else
#       include <sys/resource.h>
        rlimit core_limit = { RLIM_INFINITY, RLIM_INFINITY };
        const bool are_coredumps_enabled = setrlimit( RLIMIT_CORE, &core_limit ) == 0;
#   endif

    struct file {
        static bool exists( const std::string &pathfile) {
        /*struct stat buffer;
          return stat( pathfile.c_str(), &buffer ) == 0; */
          return access( pathfile.c_str(), F_OK ) != -1; // _access(fn,0) on win
        }
    };

    bool has( const std::string &app ) {
        return file::exists( std::string("/usr/bin/") + app );
    }

    std::string pipe( const std::string &sys, const std::string &sys2 = std::string() ) {
        char buf[512];
        std::string out;

        FILE *fp = popen( (sys+sys2).c_str(), "r" );
        if( fp ) {
            while( !std::feof(fp) ) {
                if( std::fgets(buf,sizeof(buf),fp) != NULL ) {
                    out += buf;
                }
            }
            pclose(fp);
        }

        return out;
    }

    // gdb apparently opens FD(s) 3,4,5 (whereas a typical prog uses only stdin=0, stdout=1,stderr=2)
    // Silviocesare and xorl
    bool detect_gdb(void)
    {
        bool rc = false;
        FILE *fd = fopen("/tmp", "r");

        if( fileno(fd) > 5 )
            rc = true;

        fclose(fd);
        return rc;
    }

#endif

void breakpoint() {
// os based

    $windows(
    DebugBreak();
    )

    $linux(
    raise(SIGTRAP);
//    asm("trap");
//    asm("int3");
//    kill( getpid(), SIGINT );
    /*
    kill( getpid(), SIGSTOP );
    kill( getpid(), SIGTERM );
    kill( getpid(), SIGHUP );
    kill( getpid(), SIGTRAP );
    */
    // kill( getpid(), SIGSEGV );
    // raise(SIGTRAP); //POSIX
    // raise(SIGINT);  //POSIX
    )

    $apple(
    raise(SIGTRAP);
    )

// compiler based

    //msvc
    $msvc(
    // "With native code, I prefer using int 3 vs DebugBreak, because the int 3 occurs right in the same stack frame and module as the code in question, whereas DebugBreak occurs one level down in another DLL, as shown in this callstack:"
    // [ref] http://blogs.msdn.com/b/calvin_hsia/archive/2006/08/25/724572.aspx
    // __debugbreak();
    )

    // gnuc
    $gnuc(
    //__builtin_trap();
    //__asm__ __volatile__("int3");
    )

// standard

    //abort();
    //assert( !"<heal/heal.cpp> says: debugger() has been requested" );
    // still here? duh, maybe we are in release mode...

// host based

    //macosx: asm {trap}            ; Halts a program running on PPC32 or PPC64.
    //macosx: __asm {int 3}         ; Halts a program running on IA-32.

    //$x86( // ifdef _M_X86
    //__asm int 3;
    //)
}

bool debugger( const std::string &reason )
{
    if( reason.size() > 0 )
        errorbox( reason );

// os based

    $windows(
        if( IsDebuggerPresent() ) {
            return breakpoint(), true;
        }
    )

    $linux(
        if( detect_gdb() ) {
            return breakpoint(), true;
        }
        // else try to invoke && attach to current process

        static std::string sys, tmpfile;
        sys = ( has("ddd") && false ? "/usr/bin/ddd" : ( has("gdb") ? "/usr/bin/gdb" : "" ));
        tmpfile = "./heal.tmp.tmp"; //get_pipe("tempfile");
        if( !sys.empty() ) {
            std::string pid = std::to_string( getpid() );
            // [ok]
            // eval-command=bt
            // -ex "bt full"
            // gdb --batch --quiet -ex "thread apply all bt full" -ex "quit" ${exe} ${corefile}
            sys = sys + (" --tui -q -ex 'set pagination off' -ex 'shell rm " +tmpfile+ "' -ex 'continue' -ex 'finish' -ex 'finish' -ex 'finish' --pid=") + pid + " --args `cat /proc/" + pid + "/cmdline`";
            if( has("xterm") && false ) {
                sys = std::string("/usr/bin/xterm 2>/dev/null -maximized -e \"") + sys + "\"";
            } else {
                //sys = std::string(/*"exec"*/ "/usr/bin/splitvt -upper \"") + sys + "\"";
                //sys = std::string("/bin/bash -c \"") + sys + " && /usr/bin/reset\"";
                sys = std::string("/bin/bash -c \"") + sys + "\"";
            }

        pipe( "echo heal.cpp says: waiting for debugger to catch pid > ", tmpfile );
        std::thread( system, sys.c_str() ).detach();
        while( file::exists(tmpfile) )
            usleep( 250000 );
                return true;
        }
    )

    //errorbox( "<heal/heal.cpp> says:\n\nDebugger invokation failed.\nPlease attach a debugger now.", "Error!");
    return false;
}

// ERRORBOX

namespace {

    template<typename T>
    std::string to_string( const T &t, int digits = 20 ) {
        std::stringstream ss;
        ss.precision( digits );
        ss << std::fixed << t;
        return ss.str();
    }

    template<>
    std::string to_string( const bool &boolean, int digits ) {
        return boolean ? "true" : "false";
    }

    template<>
    std::string to_string( const std::istream &is, int digits ) {
        std::stringstream ss;
        std::streamsize at = is.rdbuf()->pubseekoff(0,is.cur);
        ss << is.rdbuf();
        is.rdbuf()->pubseekpos(at);
        return ss.str();
    }

    void show( const std::string &body = std::string(), const std::string &head = std::string(), const std::string &title = std::string(), bool is_error = false ) {
        std::string headtitle = ( head.size() > 0 ? head + ": " + title : title );
        std::string headtitlebody = ( headtitle.size() > 0 ? headtitle + ": " + body : body );
        $windows(
            $no(
            int nButton;
            auto icon = is_error ? TD_ERROR_ICON : TD_WARNING_ICON;
            // TD_INFORMATION_ICON, TD_SHIELD_ICON
            std::wstring wbody( body.begin(), body.end() );
            std::wstring whead( head.begin(), head.end() );
            std::wstring wtitle( title.begin(), title.end() );
            HWND hWnd = ::GetActiveWindow(); // force modal
            TaskDialog(hWnd, NULL, wtitle.c_str(), whead.c_str(), wbody.c_str(), TDCBF_OK_BUTTON, icon, &nButton );
            )
            $yes(
            MessageBoxA( 0, body.c_str(), head.size() ? head.c_str() : "", 0 | ( is_error ? MB_ICONERROR : 0 ) | MB_SYSTEMMODAL );
            )
            return;
        )
        $linux(
            if( has("whiptail") ) {
                // gtkdialog3
                // xmessage -file ~/.bashrc -buttons "Ok:1, Cancel:2, Help:3" -print -nearmouse
                //std::string cmd = std::string("/usr/bin/zenity --information --text \"") + body + std::string("\" --title=\"") + headtitle + "\"";
                //std::string cmd = std::string("/usr/bin/dialog --title \"") + headtitle + std::string("\" --msgbox \"") + body + "\" 0 0";
                std::string cmd = std::string("/usr/bin/whiptail --title \"") + headtitle + std::string("\" --msgbox \"") + body + "\" 0 0";
                //std::string cmd = std::string("/usr/bin/xmessage \"") + headtitle + body + "\"";
                std::system( cmd.c_str() );
                return;
            }
        )
        // fallback
        std::string s;
        fprintf( stderr, "%s\n", headtitlebody.c_str() );
        std::cout << "Press enter to continue..." << std::endl;
        std::getline( std::cin, s );
    }
}

void    alert(                                                    ) { show();                        }
void    alert( const         char *text, const std::string &title ) { show( text, title );           }
void    alert( const  std::string &text, const std::string &title ) { show( text, title );           }
void    alert( const std::istream &text, const std::string &title ) { show( to_string(text), title ); }
void    alert( const       size_t &text, const std::string &title ) { show( to_string(text), title ); }
void    alert( const       double &text, const std::string &title ) { show( to_string(text), title ); }
void    alert( const        float &text, const std::string &title ) { show( to_string(text), title ); }
void    alert( const          int &text, const std::string &title ) { show( to_string(text), title ); }
void    alert( const         char &text, const std::string &title ) { show( to_string(text), title ); }
void    alert( const         bool &text, const std::string &title ) { show( to_string(text), title ); }
void errorbox( const  std::string &body, const std::string &title ) { show( body, title, "", true );  }

// DEMANGLE

#if 1
#   // Disable optimizations. Nothing gets inlined. You get pleasant stacktraces to work with.
#   // This is the default setting.
#   ifdef _MSC_VER
#       pragma optimize( "gsy", off )       // disable optimizations on msvc
#   else
#       pragma OPTIMIZE OFF                 // disable optimizations on gcc 4.4+
#   endif
#else
#   // Enable optimizations. HEAL performs better. However, functions get inlined (specially new/delete operators).
#   // This behaviour is disabled by default, since you may get wrong stacktraces.
#   ifdef _MSC_VER
#       pragma optimize( "gsy", on )        // enable optimizations on msvc
#   else
#       pragma GCC optimize                 // enable optimizations on gcc 4.4+
#       pragma optimize                     // enable optimizations on a few other compilers, hopefully
#   endif
#endif

std::string demangle( const std::string &mangled ) {
    $apple({
        std::stringstream ss;
        if( !(ss << mangled) )
            return mangled;
        std::string number, filename, address, funcname, plus, offset;
        if( !(ss >> number >> filename >> address >> funcname >> plus >> offset) )
            return mangled;
        int status = 0;
        char *demangled = abi::__cxa_demangle(funcname.c_str(), NULL, NULL, &status);
        heal::sfstring out;
        if( status == 0 && demangled ) {
            out = std::string() + demangled + " ([" + filename + "]:" + offset + ")";
        } else {
            out = std::string() + funcname  + " ([" + filename + "]:" + offset + ")";
        }
        if( demangled ) free( demangled );
        return out;
    })
    $linux({
        $no( /* c++filt way */
        FILE *fp = popen( (std::string("echo -n \"") + mangled + std::string("\" | c++filt" )).c_str(), "r" );
        if (!fp) { return mangled; }
        char demangled[1024];
        char *line_p = fgets(demangled, sizeof(demangled), fp);
        pclose(fp);
        return demangled;
        )
        $yes( /* addr2line way. wip & quick proof-of-concept. clean up required. */
        heal::sfstring binary = mangled.substr( 0, mangled.find_first_of('(') );
        heal::sfstring address = mangled.substr( mangled.find_last_of('[') + 1 );
        address.pop_back();
        heal::sfstring cmd( "addr2line -e \1 \2", binary, address );
        FILE *fp = popen( cmd.c_str(), "r" );
        if (!fp) { return mangled; }
        char demangled[1024];
        char *line_p = fgets(demangled, sizeof(demangled), fp);
        pclose(fp);
        heal::sfstring demangled_(demangled);
        if( demangled_.size() ) demangled_.pop_back(); //remove \n
        return demangled_.size() && demangled_.at(0) == '?' ? mangled : demangled_;
        )
    })
    $windows({
        char demangled[1024];
        return (UnDecorateSymbolName(mangled.c_str(), demangled, sizeof( demangled ), UNDNAME_COMPLETE)) ? std::string(demangled) : mangled;
    })
    /*
    $gnuc({
        std::string out;
        int status = 0;
        char *demangled = abi::__cxa_demangle(mangled.c_str(), 0, 0, &status);
        out = ( status == 0 && demangled ? std::string(demangled) : mangled );
        if( demangled ) free( demangled );
        return out;
    })
    */
        return mangled;
}

// CALLSTACK

        callstack::callstack( bool autosave ) {
            if( autosave ) save();
        }

        size_t callstack::space() const {
            return sizeof(frames) + sizeof(void *) * frames.size();
        }

        void callstack::save( unsigned frames_to_skip ) {

            if( frames_to_skip > max_frames )
                return;

            frames.clear();
            frames.resize( max_frames, (void *)0 );
            void **out_frames = &frames[0]; // .data();

            $windows({
                unsigned short capturedFrames = 0;

                // RtlCaptureStackBackTrace is only available on Windows XP or newer versions of Windows
                typedef WORD(NTAPI FuncRtlCaptureStackBackTrace)(DWORD, DWORD, PVOID *, PDWORD);

                static struct raii
                {
                    raii() : module(0), ptrRtlCaptureStackBackTrace(0)
                    {
                        module = LoadLibraryA("kernel32.dll");
                        if( !module )
                            fail( "<heal/heal.cpp> says: error! cant load kernel32.dll" );

                        ptrRtlCaptureStackBackTrace = (FuncRtlCaptureStackBackTrace *)GetProcAddress(module, "RtlCaptureStackBackTrace");
                        if( !ptrRtlCaptureStackBackTrace )
                            fail( "<heal/heal.cpp> says: error! cant find RtlCaptureStackBackTrace() process address" );
                    }
                    ~raii() { if(module) FreeLibrary(module); }

                    HMODULE module;
                    FuncRtlCaptureStackBackTrace *ptrRtlCaptureStackBackTrace;
                } module;

                if( module.ptrRtlCaptureStackBackTrace )
                    capturedFrames = module.ptrRtlCaptureStackBackTrace(frames_to_skip+1, max_frames, out_frames, (DWORD *) 0);

                frames.resize( capturedFrames );
                std::vector<void *>(frames).swap(frames);
                return;
            })
            $gnuc({
                // Ensure the output is cleared
                std::memset(out_frames, 0, (sizeof(void *)) * max_frames);

                frames.resize( backtrace(out_frames, max_frames) );
                std::vector<void *>(frames).swap(frames);
                return;
            })
        }

        std::vector<std::string> callstack::unwind( std::size_t from, std::size_t to ) const
        {
            if( to == ~0 )
                to = this->frames.size();

            if( from > to || from > this->frames.size() || to > this->frames.size() )
                return std::vector<std::string>();

            const size_t num_frames = to - from;
            std::vector<std::string> backtraces( num_frames );

            void * const * frames = &this->frames[ from ];
            const std::string invalid = "????";

            $windows({
                SymSetOptions(SYMOPT_UNDNAME);

                $no(
                    // polite version. this is how things should be done.
                    HANDLE process = GetCurrentProcess();
                    if( SymInitialize( process, NULL, TRUE ) )
                )
                $yes(
                    // this is what we have to do because other memory managers are not polite enough. fuck them off
                    static HANDLE process = GetCurrentProcess();
                    static int init = SymInitialize( process, NULL, TRUE );
                    if( !init )
                        fail( "<heal/heal.cpp> says: cannot initialize Dbghelp.lib" );
                )
                {
                    enum { MAXSYMBOLNAME = 512 - sizeof(IMAGEHLP_SYMBOL64) };
                    char symbol64_buf     [ 512 ];
                    char symbol64_bufblank[ 512 ] = {0};
                    IMAGEHLP_SYMBOL64 *symbol64       = reinterpret_cast<IMAGEHLP_SYMBOL64*>(symbol64_buf);
                    IMAGEHLP_SYMBOL64 *symbol64_blank = reinterpret_cast<IMAGEHLP_SYMBOL64*>(symbol64_bufblank);
                    symbol64_blank->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
                    symbol64_blank->MaxNameLength = (MAXSYMBOLNAME-1) / 2; //wchar?

                    IMAGEHLP_LINE64 line64, line64_blank = {0};
                    line64_blank.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

                    for( unsigned i = 0; i < num_frames; i++ ) {
                        *symbol64 = *symbol64_blank;
                        DWORD64 displacement64 = 0;

                        if( SymGetSymFromAddr64( process, (DWORD64) frames[i], &displacement64, symbol64 ) ) {
                            line64 = line64_blank;
                            DWORD displacement = 0;
                            if( SymGetLineFromAddr64( process, (DWORD64) frames[i], &displacement, &line64 ) ) {
                                backtraces[i] = heal::sfstring( "\1 (\2:\3)", symbol64->Name, line64.FileName, line64.LineNumber );
                            } else {
                                backtraces[i] = symbol64->Name;
                            }
                        } else  backtraces[i] = invalid;
                    }

                    $no(
                        // fuck the others. cleanup commented.
                        SymCleanup(process);
                    )
                }
                DWORD error = GetLastError();

                return backtraces;
            })
            $gnuc({
                char **strings = backtrace_symbols(frames, num_frames);

                // Decode the strings
                if( strings ) {
                    for( unsigned i = 0; i < num_frames; i++ ) {
                        backtraces[i] = ( strings[i] ? demangle(strings[i]) : invalid );
                    }
                    free( strings );
                }

                return backtraces;
            })

            return backtraces;
        }

        std::vector<std::string> callstack::str( const char *format12, size_t skip_begin ) const {
            callstack copy = *this;
            std::vector<std::string> stacktrace = copy.unwind( skip_begin );

            for( size_t i = 0, end = stacktrace.size(); i < end; i++ )
                stacktrace[i] = heal::sfstring( format12, i + 1, stacktrace[i] );

            return stacktrace;
        }

        std::string callstack::flat( const char *format12, size_t skip_begin ) const {
            std::vector<std::string> vec = str( format12, skip_begin );
            std::string str;
            for( std::vector<std::string>::const_iterator it = vec.begin(), end = vec.end(); it != end; ++it ) {
                str += *it;
            }
            return str;
        }

std::vector<std::string> stacktrace( const char *format12, size_t skip_initial ) {
    return callstack(true).str( format12, skip_initial );
}

std::string stackstring( const char *format12, size_t skip_initial ) {
    std::string out;
    std::vector<std::string> stack = stacktrace( format12, skip_initial );
    for( std::vector<std::string>::const_iterator it = stack.begin(), end = stack.end(); it != end; ++it ) {
        out += *it;
    }
    return out;
}

// DIE

void die( const std::string &reason, int errorcode )
{
    if( !reason.empty() ) {
        fail( reason );
    }

    $windows(
    FatalExit( errorcode );
    )

    // fallback
    std::exit( errorcode );
}

void die( int errorcode, const std::string &reason )
{
    die( reason, errorcode );
}

// HEXDUMP
// @todo: maxwidth != 80 doesnt work

std::string hexdump( const void *data, size_t num_bytes, const void *self )
{
#   ifdef _MSC_VER
#       pragma warning( push )
#       pragma warning( disable : 4996 )
#       define $vsnprintf _vsnprintf
#   else
#       define $vsnprintf  vsnprintf
#   endif

    struct local {
        static std::string format( const char *fmt, ... )
        {
            int len;
            std::string self;

            using namespace std;

            // Calculate the final length of the formatted string
            {
                va_list args;
                va_start( args, fmt );
                len = $vsnprintf( 0, 0, fmt, args );
                va_end( args );
            }

            // Allocate a buffer (including room for null termination
            char* target_string = new char[++len];

            // Generate the formatted string
            {
                va_list args;
                va_start( args, fmt );
                $vsnprintf( target_string, len, fmt, args );
                va_end( args );
            }

            // Assign the formatted string
            self.assign( target_string );

            // Clean up
            delete [] target_string;

            return self;
        }
    };

    unsigned maxwidth = 80;
    unsigned width = 16; //column width
    unsigned width_offset_block = (8 + 1);
    unsigned width_chars_block  = (width * 3 + 1) + sizeof("asc");
    unsigned width_hex_block    = (width * 3 + 1) + sizeof("hex");
    unsigned width_padding = (std::max)( 0, int( maxwidth - ( width_offset_block + width_chars_block + width_hex_block ) ) );
    unsigned blocks = width_padding / ( width_chars_block + width_hex_block ) ;

    std::size_t dumpsize = ( num_bytes < width * 16 ? num_bytes : width * 16 ); //16 lines max

    std::string result;

    result += local::format( "%-*s %-.*s [ptr=%p sz=%d]\n", width_offset_block - 1, "offset", width_chars_block - 1, "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F", self ? self : data, num_bytes );

    if( !num_bytes )
        return result;

    blocks++;

    const unsigned char *p = reinterpret_cast<const unsigned char *>( data );
    size_t i = 0;

    while( i < dumpsize )
    {
        //offset block
        result += local::format("%p ", (size_t)(p + i) ); //%08x, %08zx

        //chars blocks
        for( unsigned b = 0; b < blocks; b++)
        {
            for( unsigned c = 0 ; c < width ; c++ )
                result += local::format(" %c ", i + c >= dumpsize ? '.' : p[i + c] < 32 || p[i + c] >= 127 ? '.' : p[i + c]);

            result += "asc\n";
        }

        //offset block
        result += local::format("%p ", (size_t)(p + i) ); //%08x, %08zx

        //hex blocks
        for( unsigned b = 0; b < blocks; b++)
        {
            for( unsigned c = 0; c < width ; c++)
                result += local::format( i + c < dumpsize ? "%02x " : "?? ", p[i + c]);

            result += "hex\n";
        }

        //next line
        //result += '\n';
        i += width * blocks;
    }

    return result;

#   undef $vsnprintf
#   ifdef _MSC_VER
#       pragma warning( pop )
#   endif
}

std::string timestamp() {
    std::stringstream ss;
    ss << __TIMESTAMP__;
    return ss.str();
}

/*
 * Simple prompt dialog (based on legolas558's code). zlib/libpng license.
 * - rlyeh
 */

#if !$on($windows)

std::string prompt( const std::string &current_value, const std::string &title, const std::string &caption )
{
    std::string out;

    if( has("whiptail") && false )
    {
        std::string out = pipe( std::string() +
            "/usr/bin/whiptail 3>&1 1>&2 2>&3 --title \"" +title+ "\" --inputbox \"" +caption+ "\" 0 0 \"" +current_value+ "\"" );
    }
    else
    {
        if( title.size() && caption.size() )
            fprintf(stdout, "%s", title.c_str());
        else
            fprintf(stdout, "%s", title.empty() ? caption.c_str() : title.c_str());

        if( !current_value.empty() )
            fprintf(stdout, " (enter defaults to '%s')", current_value.c_str());

        fprintf(stdout, "%s", "\n");

        std::getline( std::cin, out );

        if( out.empty() )
            out = current_value;
    }

    return out;
}

#endif

#if $on($msvc)
#   pragma warning( push )
#   pragma warning( disable : 4996 )
#endif

#if $on($windows)
#   pragma comment(lib,"user32.lib")
#   pragma comment(lib,"gdi32.lib")
//$warning("<heal/heal.cpp> says: dialog aware dpi fix (@todo)")

std::string prompt( const std::string &current_value, const std::string &title, const std::string &caption )
{
    class InputBox
    {
        private:

        HWND                hwndParent,
                            hwndInputBox,
                            hwndQuery,
                            hwndOk,
                            hwndCancel,
                            hwndEditBox;
        LPSTR               szInputText;
        WORD                wInputMaxLength, wInputLength;
        bool                bRegistered,
                            bResult;

        HINSTANCE           hThisInstance;

        enum
        {
            CIB_SPAN = 10,
            CIB_LEFT_OFFSET = 6,
            CIB_TOP_OFFSET = 4,
            CIB_WIDTH = 300,
            CIB_HEIGHT = 130,
            CIB_BTN_WIDTH = 60,
            CIB_BTN_HEIGHT = 20
        };

        public:

#       define CIB_CLASS_NAME   "CInputBoxA"

        static LRESULT CALLBACK CIB_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            InputBox *self;
            self = (InputBox *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

            switch (msg)
            {
                case WM_CREATE:
                    self = (InputBox *) ((CREATESTRUCT *)lParam)->lpCreateParams;
                    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)self);
                    self->create(hWnd);
                break;
                case WM_COMMAND:
                    switch(LOWORD(wParam)) {
                        case IDOK:
                            self->submit();
                        case IDCANCEL:
                            self->close();
                        break;
                    }
                    break;
                case WM_CLOSE:
                    self->close();
                    return 0;
                case WM_DESTROY:
                    self->destroy();
                    break;
            }
            return(DefWindowProc (hWnd, msg, wParam, lParam));
        }

        InputBox( HINSTANCE hInst ) :
            hwndParent(0),
            hwndInputBox(0),
            hwndQuery(0),
            hwndOk(0),
            hwndCancel(0),
            hwndEditBox(0),
            szInputText(0),
            wInputMaxLength(0), wInputLength(0),
            bRegistered(false),
            bResult(false),
            hThisInstance(hInst)
        {
            WNDCLASSEXA wndInputBox;
            RECT rect;

            memset(&wndInputBox, 0, sizeof(WNDCLASSEXA));

            hThisInstance = hInst;

            wndInputBox.cbSize                  = sizeof(wndInputBox);
            wndInputBox.lpszClassName           = CIB_CLASS_NAME;
            wndInputBox.style                   = CS_HREDRAW | CS_VREDRAW;
            wndInputBox.lpfnWndProc             = CIB_WndProc;
            wndInputBox.lpszMenuName            = NULL;
            wndInputBox.hIconSm                 = NULL;
            wndInputBox.cbClsExtra              = 0;
            wndInputBox.cbWndExtra              = 0;
            wndInputBox.hInstance               = hInst;
            wndInputBox.hIcon                   = LoadIcon(NULL, IDI_WINLOGO);
            wndInputBox.hCursor                 = LoadCursor(NULL, IDC_ARROW);
            wndInputBox.hbrBackground           = (HBRUSH)(COLOR_WINDOW);

            RegisterClassExA(&wndInputBox);

            if (hwndParent)
                GetWindowRect(hwndParent, &rect); //always false?
            else
                GetWindowRect(GetDesktopWindow(), &rect);

            hwndInputBox = CreateWindowA( CIB_CLASS_NAME, "",
                            (WS_BORDER | WS_CAPTION), rect.left+(rect.right-rect.left-CIB_WIDTH)/2,
                            rect.top+(rect.bottom-rect.top-CIB_HEIGHT)/2,
                            CIB_WIDTH, CIB_HEIGHT, hwndParent, NULL,
                            hThisInstance, this);
        }

        void destroy()
        {
            EnableWindow(hwndParent, true);
            SendMessage(hwndInputBox, WM_CLOSE/*WM_DESTROY*/, 0, 0);
        }

        ~InputBox()
        {
            UnregisterClassA(CIB_CLASS_NAME, hThisInstance);
        }

        void submit()
        {
            wInputLength = (int)SendMessage(hwndEditBox, EM_LINELENGTH, 0, 0);
            if (wInputLength) {
                *((LPWORD)szInputText) = wInputMaxLength;
                wInputLength = (WORD)SendMessage(hwndEditBox, EM_GETLINE, 0, (LPARAM)szInputText);
            }
            szInputText[wInputLength] = '\0';
            bResult = true;
        }

        void create(HWND hwndNew)
        {
            static HFONT myFont = NULL;

            if( myFont != NULL )
            {
                DeleteObject( myFont );
                myFont = NULL;
            }

            hwndInputBox = hwndNew;

            NONCLIENTMETRICS ncm;
            ncm.cbSize = sizeof(NONCLIENTMETRICS);

            if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
            {
#if 0
                LOGFONT lf;
                memset(&lf,0,sizeof(LOGFONT));

                lf.lfWeight= FW_NORMAL;
                lf.lfCharSet= ANSI_CHARSET;
                //lf.lfPitchAndFamily = 35;
                lf.lfHeight= 10;
                strcpy(lf.lfFaceName, "Tahoma");
                myFont=CreateFontIndirect(&lf);
#else
                myFont = CreateFontIndirect(&ncm.lfMessageFont);
#endif
            }
            else
            {
                myFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            }

            //  SetWindowPos(hwndInputBox, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            hwndQuery = CreateWindowA("Static", "", WS_CHILD | WS_VISIBLE,
                                    CIB_LEFT_OFFSET, CIB_TOP_OFFSET,
                                    CIB_WIDTH-CIB_LEFT_OFFSET*2, CIB_BTN_HEIGHT*2,
                                    hwndInputBox, NULL,
                                    hThisInstance, NULL);
            hwndEditBox = CreateWindowA("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, CIB_LEFT_OFFSET,
                                    CIB_TOP_OFFSET + CIB_BTN_HEIGHT*2, CIB_WIDTH-CIB_LEFT_OFFSET*3, CIB_BTN_HEIGHT,
                                    hwndInputBox,   NULL,
                                    hThisInstance, NULL);
            hwndOk = CreateWindowA("Button", "OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                    CIB_WIDTH/2 - CIB_SPAN*2 - CIB_BTN_WIDTH, CIB_HEIGHT - CIB_TOP_OFFSET*4 - CIB_BTN_HEIGHT*2,
                                    CIB_BTN_WIDTH, CIB_BTN_HEIGHT, hwndInputBox, (HMENU)IDOK,
                                    hThisInstance, NULL);
            hwndCancel = CreateWindowA("Button", "Cancel",
                                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                    CIB_WIDTH/2 + CIB_SPAN, CIB_HEIGHT - CIB_TOP_OFFSET*4 - CIB_BTN_HEIGHT*2,  CIB_BTN_WIDTH, CIB_BTN_HEIGHT,
                                    hwndInputBox, (HMENU)IDCANCEL,
                                    hThisInstance, NULL);

        //  SendMessage(hwndInputBox,WM_SETFONT,(WPARAM)myFont,FALSE);
            SendMessage(hwndQuery,WM_SETFONT,(WPARAM)myFont,FALSE);
            SendMessage(hwndEditBox,WM_SETFONT,(WPARAM)myFont,FALSE);
            SendMessage(hwndOk,WM_SETFONT,(WPARAM)myFont,FALSE);
            SendMessage(hwndCancel,WM_SETFONT,(WPARAM)myFont,FALSE);
        }

        void close()
        {
            PostMessage(hwndInputBox, WM_CLOSE, 0, 0);
        }

        void hide()
        {
            ShowWindow(hwndInputBox, SW_HIDE);
        }

        void show(LPCSTR lpszTitle, LPCSTR  lpszQuery)
        {
            SetWindowTextA(hwndInputBox, lpszTitle);
            SetWindowTextA(hwndEditBox, szInputText);
            SetWindowTextA(hwndQuery, lpszQuery);
            SendMessage(hwndEditBox, EM_LIMITTEXT, wInputMaxLength, 0);
            SendMessage(hwndEditBox, EM_SETSEL, 0, -1);
            SetFocus(hwndEditBox);
            ShowWindow(hwndInputBox, SW_NORMAL);
        }

        int show(HWND hwndParentWindow, LPCSTR lpszTitle, LPCSTR lpszQuery, LPSTR szResult, WORD wMax)
        {
            MSG msg;
            BOOL    bRet;
            hwndParent = hwndParentWindow;
            szInputText = szResult;
            wInputMaxLength = wMax;

            bResult = false;

        //  EnableWindow(hwndParent, false);

            show(lpszTitle, lpszQuery);

            while( (bRet = GetMessageA( &msg, NULL, 0, 0 )) != 0)
            {
                if (msg.message==WM_KEYDOWN) {
                    switch (msg.wParam) {
                    case VK_RETURN:
                        submit();
                    case VK_ESCAPE:
                        close();
                        break;
                    default:
                        TranslateMessage(&msg);
                        break;
                    }
                } else
        //      if (!IsDialogMessage(hwndInputBox, &msg)) {
                    TranslateMessage(&msg);
        //      }
                DispatchMessage(&msg);
                if (msg.message == WM_CLOSE)
                    break;
            }

        //  EnableWindow(hwndParent, true);

            return bResult;
        }

    #   undef CIB_CLASS_NAME
    }
    myinp(GetModuleHandle(0));

    char *result = new char [2048+1];

    memset( result, 0, 2048+1 ); //default value

    strcpy( result, current_value.c_str() );

    myinp.show(0, "L", caption.size() ? caption.c_str() : title.c_str(), result, 2048);

    std::string _r = result;

    delete [] result;

    return _r;
}

#endif

}

#if $on($msvc)
#   pragma warning( pop )
#endif

/*
#undef $debug
#undef $release
#undef $other
#undef $gnuc
#undef $msvc

#undef $undefined
#undef $apple
#undef $linux
#undef $windows

#undef $no
#undef $yes
*/


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

//#line 1 "route66.cpp"
// lightweight http server
// - rlyeh, zlib/libpng licensed. based on code by Ivan Tikhonov (zlib licensed)

// Sockets, taken from https://github.com/r-lyeh/knot

#if defined(_WIN32)

#   include <winsock2.h>
#   include <ws2tcpip.h>
#   include <windows.h>

#   ifndef _MSC_VER
    static
    const char* inet_ntop(int af, const void* src, char* dst, int cnt){
        struct sockaddr_in srcaddr;
        memset(&srcaddr, 0, sizeof(struct sockaddr_in));
        memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));
        srcaddr.sin_family = af;
        if (WSAAddressToString((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0) {
            DWORD rv = WSAGetLastError();
            return NULL;
        }
        return dst;
    }
#   endif

#   pragma comment(lib,"ws2_32.lib")

#   define INIT()                    do { static WSADATA wsa_data; static const int init = WSAStartup( MAKEWORD(2, 2), &wsa_data ); } while(0)
#   define SOCKET(A,B,C)             ::socket((A),(B),(C))
#   define ACCEPT(A,B,C)             ::accept((A),(B),(C))
#   define CONNECT(A,B,C)            ::connect((A),(B),(C))
#   define CLOSE(A)                  ::closesocket((A))
#   define RECV(A,B,C,D)             ::recv((A), (char *)(B), (C), (D))
#   define READ(A,B,C)               ::recv((A), (char *)(B), (C), (0))
#   define SELECT(A,B,C,D,E)         ::select((A),(B),(C),(D),(E))
#   define SEND(A,B,C,D)             ::send((A), (const char *)(B), (int)(C), (D))
#   define WRITE(A,B,C)              ::send((A), (const char *)(B), (int)(C), (0))
#   define GETSOCKOPT(A,B,C,D,E)     ::getsockopt((A),(B),(C),(char *)(D), (int*)(E))
#   define SETSOCKOPT(A,B,C,D,E)     ::setsockopt((A),(B),(C),(char *)(D), (int )(E))

#   define BIND(A,B,C)               ::bind((A),(B),(C))
#   define LISTEN(A,B)               ::listen((A),(B))
#   define SHUTDOWN(A)               ::shutdown((A),2)
#   define SHUTDOWN_R(A)             ::shutdown((A),0)
#   define SHUTDOWN_W(A)             ::shutdown((A),1)

    namespace
    {
        // fill missing api

        enum
        {
            F_GETFL = 0,
            F_SETFL = 1,

            O_NONBLOCK = 128 // dummy
        };

        int fcntl( int &sockfd, int mode, int value )
        {
            if( mode == F_GETFL ) // get socket status flags
                return 0; // original return current sockfd flags

            if( mode == F_SETFL ) // set socket status flags
            {
                u_long iMode = ( value & O_NONBLOCK ? 0 : 1 );

                bool result = ( ioctlsocket( sockfd, FIONBIO, &iMode ) == NO_ERROR );

                return 0;
            }

            return 0;
        }
    }

#else

#   include <fcntl.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netdb.h>
#   include <unistd.h>    //close

#   include <arpa/inet.h> //inet_addr, inet_ntop

#   define INIT()                    do {} while(0)
#   define SOCKET(A,B,C)             ::socket((A),(B),(C))
#   define ACCEPT(A,B,C)             ::accept((A),(B),(C))
#   define CONNECT(A,B,C)            ::connect((A),(B),(C))
#   define CLOSE(A)                  ::close((A))
#   define READ(A,B,C)               ::read((A),(B),(C))
#   define RECV(A,B,C,D)             ::recv((A), (void *)(B), (C), (D))
#   define SELECT(A,B,C,D,E)         ::select((A),(B),(C),(D),(E))
#   define SEND(A,B,C,D)             ::send((A), (const int8 *)(B), (C), (D))
#   define WRITE(A,B,C)              ::write((A),(B),(C))
#   define GETSOCKOPT(A,B,C,D,E)     ::getsockopt((int)(A),(int)(B),(int)(C),(      void *)(D),(socklen_t *)(E))
#   define SETSOCKOPT(A,B,C,D,E)     ::setsockopt((int)(A),(int)(B),(int)(C),(const void *)(D),(int)(E))

#   define BIND(A,B,C)               ::bind((A),(B),(C))
#   define LISTEN(A,B)               ::listen((A),(B))
#   define SHUTDOWN(A)               ::shutdown((A),SHUT_RDWR)
#   define SHUTDOWN_R(A)             ::shutdown((A),SHUT_RD)
#   define SHUTDOWN_W(A)             ::shutdown((A),SHUT_WR)

#endif

#include <cstdlib>
#include <cstring>
#include <limits.h>

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>


//#line 1 "route66.hpp"
// lightweight http server
// - rlyeh, zlib/libpng licensed. based on code by Ivan Tikhonov (zlib licensed)

/**/// <-- remove an asterisk in a code editor to highlight API

//#pragma once

#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <sstream>

// taken from https://github.com/r-lyeh/bridge {
#if (__cplusplus < 201103L && !defined(_MSC_VER)) || (defined(_MSC_VER) && (_MSC_VER < 1700)) || (defined(__GLIBCXX__) && __GLIBCXX__ < 20130322L)
#   include <boost/functional.hpp> // if old libstdc++ or msc libs are found, use boost::function
#   include <boost/function.hpp>   //
#   include <boost/thread.hpp>     // and boost::thread
namespace std {
    namespace placeholders {
        //...
    }
    using namespace boost;
}
#else
#   include <functional>       // else assume modern c++11 and use std::function<> instead
#   include <mutex>            // and std::mutex
#   include <thread>           // and std::thread
#endif
// }

namespace route66 {

    /*/ request:: 'class'
    /*/
    struct request {
        std::string data;                              /*/      .data 'raw received data, as is' /*/
        std::string method;                            /*/    .method 'raw method ("GET HEAD POST PUT ...")' /*/
        std::string url;                               /*/       .url 'raw request ("/search?req1=blabla&req2=what%20ever")' /*/
        std::string uri;                               /*/      .path 'decoded path uri ("/search")' /*/
        std::string options;                           /*/   .options 'raw GET/POST options url ("req1=blabla&req2=what%20ever")' /*/
        std::map<std::string, std::string> arguments;  /*/ .arguments 'decoded options decoupled as arguments ( {"req1","blabla"}, {"req2","what ever"} )' /*/
        std::map<std::string, std::string> multipart;  /*/ .multipart 'multipart files when POST' /*/
        std::string ip;                                /*/        .ip 'ip address of request' /*/
           unsigned port;                              /*/      .port 'number port of request' /*/
        std::string str() const {
            typedef std::map<std::string, std::string>::const_iterator cit;
            std::stringstream ss;
            ss << "route66::request[" << this << "] = {" << std::endl;
            ss << "\t" "ip: " << ip << std::endl;
            ss << "\t" "port: " << port << std::endl;
            ss << "\t" "method: " << method << std::endl;
            ss << "\t" "options: " << options << std::endl;
            ss << "\t" "uri: " << uri << std::endl;
            ss << "\t" "url: " << url << std::endl;
            for( cit it = arguments.begin(), end = arguments.end(); it != end; ++it ) {
                ss << "\t" "argument[" << it->first << "]: " << it->second << std::endl;
            }
            for( cit it = multipart.begin(), end = multipart.end(); it != end; ++it ) {
                ss << "\t" "multipart[" << it->first << "]: " << it->second << std::endl;
            }
            ss << "\t" "data: " << data << std::endl;
            ss << "}; //route66::request [" << this << "]" << std::endl;
            return ss.str();
        }
    };

    /*/ callback; 'alias'
    /*/ typedef std::function< int(request &,std::ostream &,std::ostream &) > callback;

    /*/ create() 'start a daemon at given port and route mask, callback will be invoked; returns false if failed.'
    /*/ bool create( unsigned port, const std::string &mask, const route66::callback &callback );

    /*/ fserve() 'start a file server daemon at given port and relative path, all file uri contents will be returned; returns false if failed.'
    /*/ bool fserve( unsigned port, const std::string &mask, const std::string &relpath );

    /*/ httpcode:: 'listing of HTTP result codes'
    /*/
    struct httpcode {
        unsigned code;
        const char *const description;
    };

    /*/ 0XX 'route66'
    /*/
    extern const httpcode SHUTDOWN;
    /*/ 1XX 'informational'
    /*/
    extern const httpcode CONTINUE;
    extern const httpcode SWITCH_PROTOCOLS;
    /*/ 2XX 'success'
    /*/
    extern const httpcode OK;
    extern const httpcode CREATED;
    extern const httpcode ACCEPTED;
    extern const httpcode PARTIAL;
    extern const httpcode NO_CONTENT;
    extern const httpcode RESET_CONTENT;
    extern const httpcode PARTIAL_CONTENT;
    extern const httpcode WEBDAV_MULTI_STATUS;
    /*/ 3XX 'redirection'
    /*/
    extern const httpcode AMBIGUOUS;
    extern const httpcode MOVED;
    extern const httpcode REDIRECT;
    extern const httpcode REDIRECT_METHOD;
    extern const httpcode NOT_MODIFIED;
    extern const httpcode USE_PROXY;
    extern const httpcode REDIRECT_KEEP_VERB;
    /*/ 4XX 'client error'
    /*/
    extern const httpcode BAD_REQUEST;
    extern const httpcode DENIED;
    extern const httpcode PAYMENT_REQ;
    extern const httpcode FORBIDDEN;
    extern const httpcode NOT_FOUND;
    extern const httpcode BAD_METHOD;
    extern const httpcode NONE_ACCEPTABLE;
    extern const httpcode PROXY_AUTH_REQ;
    extern const httpcode REQUEST_TIMEOUT;
    extern const httpcode CONFLICT;
    extern const httpcode GONE;
    extern const httpcode LENGTH_REQUIRED;
    extern const httpcode PRECOND_FAILED;
    extern const httpcode REQUEST_TOO_LARGE;
    extern const httpcode URI_TOO_LONG;
    extern const httpcode UNSUPPORTED_MEDIA;
    extern const httpcode RETRY_WITH;
    /*/ 5XX 'server error'
    /*/
    extern const httpcode SERVER_ERROR;
    extern const httpcode NOT_SUPPORTED;
    extern const httpcode BAD_GATEWAY;
    extern const httpcode SERVICE_UNAVAIL;
    extern const httpcode GATEWAY_TIMEOUT;
    extern const httpcode VERSION_NOT_SUP;

    /*/ mime() 'MIME string header based on file extension (ie, sample.json -> "Content-Type: application/json; charset=utf-8\r\n" )' /*/
    /*/ tag()  'MIME string tag based on file extension (ie, sample.json -> "application/json" )' /*/
    std::string mime( const std::string &filename_ext );
    std::string tag( const std::string &filename_ext );
}

namespace {
    // http://stackoverflow.com/questions/2673207/c-c-url-decode-library
    char * urldecode2(char *dst, const char *src) {
        char a, b;
        while (*src) {
            if ((*src == '%') &&
                ((a = src[1]) && (b = src[2])) &&
                (isxdigit(a) && isxdigit(b))) {
                    if (a >= 'a')
                            a -= 'a'-'A';
                    if (a >= 'A')
                            a -= ('A' - 10);
                    else
                            a -= '0';
                    if (b >= 'a')
                            b -= 'a'-'A';
                    if (b >= 'A')
                            b -= ('A' - 10);
                    else
                            b -= '0';
                    *dst++ = 16*a+b;
                    src+=3;
            } else {
                *dst++ = *src++;
            }
        }
        *dst++ = '\0';
        return dst - 1;
    }

    std::string &urldecode2( std::string &url ) {
        char *begin = &url[0], *end = urldecode2( begin, begin );
        url.resize( end - begin );
        return url;
    }

    // taken from https://github.com/r-lyeh/wire
    std::string replace( std::string s, const std::string &target, const std::string &repl ) {
        for( size_t it = 0, tlen = target.length(), rlen = repl.length(); ( it = s.find( target, it ) ) != std::string::npos; it += rlen ) {
            s.replace( it, tlen, repl );
        }
        return s;
    };
    std::vector< std::string > tokenize( const std::string &self, const std::string &delimiters ) {
        std::string map( 256, '\0' );
        for( std::string::const_iterator it = delimiters.begin(), end = delimiters.end(); it != end; ++it ) {
            unsigned char ch( *it );
            map[ ch ] = '\1';
        }
        std::vector< std::string > tokens(1);
        for( std::string::const_iterator it = self.begin(), end = self.end(); it != end; ++it ) {
            unsigned char ch( *it );
            /**/ if( !map.at(ch)          ) tokens.back().push_back( char(ch) );
            else if( tokens.back().size() ) tokens.push_back( std::string() );
        }
        while( tokens.size() && !tokens.back().size() ) tokens.pop_back();
        return tokens;
    }
    bool match( const char *pattern, const char *str ) {
        if( *pattern=='\0' ) return !*str;
        if( *pattern=='*' )  return match(pattern+1, str) || (*str && match(pattern, str+1));
        if( *pattern=='?' )  return *str && (*str != '.') && match(pattern+1, str+1);
        return (*str == *pattern) && match(pattern+1, str+1);
    }
    bool matches( const std::string &self, const std::string &pattern ) {
        return match( pattern.c_str(), self.c_str() );
    }
    bool endswith( const std::string &self, const std::string &eof ) {
        return eof.size() < self.size() && self.substr( self.size() - eof.size() ) == eof;
    }
}

namespace route66 {

std::string tag( const std::string &pathfile ) {
    static std::map< std::string, std::string> mimes;
    if( mimes.empty() ) {
        // [ref] taken from http://en.wikipedia.org/wiki/Internet_media_type

        mimes[".avi"] = "video/avi";                  // Covers most Windows-compatible formats including .avi and .divx[16]
        mimes[".mpg"] = "video/mpeg";                 // MPEG-1 video with multiplexed audio; Defined in RFC 2045 and RFC 2046
        mimes[".mpeg"] = "video/mpeg";                // MPEG-1 video with multiplexed audio; Defined in RFC 2045 and RFC 2046
        mimes[".mp4"] = "video/mp4";                  // MP4 video; Defined in RFC 4337
        mimes[".mov"] = "video/quicktime";            // QuickTime video; Registered[17]
        mimes[".webm"] = "video/webm";                // WebM Matroska-based open media format
        mimes[".mkv"] = "video/x-matroska";           // Matroska open media format
        mimes[".wmv"] = "video/x-ms-wmv";             // Windows Media Video; Documented in Microsoft KB 288102
        mimes[".flv"] = "video/x-flv";                // Flash video (FLV files)

        mimes[".css"] = "text/css";                   // Cascading Style Sheets; Defined in RFC 2318
        mimes[".csv"] = "text/csv";                   // Comma-separated values; Defined in RFC 4180
        mimes[".htm"] = "text/html";                  // HTML; Defined in RFC 2854
        mimes[".html"] = "text/html";                 // HTML; Defined in RFC 2854
        mimes[".txt"] = "text/plain";                 // Textual data; Defined in RFC 2046 and RFC 3676
        mimes[".text"] = "text/plain";                // Textual data; Defined in RFC 2046 and RFC 3676
        mimes[".rtf"] = "text/rtf";                   // RTF; Defined by Paul Lindner
        mimes[".xml"] = "text/xml";                   // Extensible Markup Language; Defined in RFC 3023
        mimes[".tsv"] = "text/tab-separated-values";

        mimes[".gif"] = "image/gif";                  // GIF image; Defined in RFC 2045 and RFC 2046
        mimes[".jpg"] = "image/jpeg";                 // JPEG JFIF image; Defined in RFC 2045 and RFC 2046
        mimes[".jpeg"] = "image/jpeg";                // JPEG JFIF image; Defined in RFC 2045 and RFC 2046
        mimes[".pjpg"] = "image/pjpeg";               // JPEG JFIF image; Associated with Internet Explorer; Listed in ms775147(v=vs.85) - Progressive JPEG, initiated before global browser support for progressive JPEGs (Microsoft and Firefox).
        mimes[".pjpeg"] = "image/pjpeg";              // JPEG JFIF image; Associated with Internet Explorer; Listed in ms775147(v=vs.85) - Progressive JPEG, initiated before global browser support for progressive JPEGs (Microsoft and Firefox).
        mimes[".png"] = "image/png";                  // Portable Network Graphics; Registered,[13] Defined in RFC 2083
        mimes[".svg"] = "image/svg+xml";              // SVG vector image; Defined in SVG Tiny 1.2 Specification Appendix M
        mimes[".psd"] = "image/vnd.adobe.photoshop";

        mimes[".aif"] = "audio/x-aiff";
        mimes[".s3m"] = "audio/s3m";
        mimes[".xm"] = "audio/xm";
        mimes[".snd"] = "audio/x-adpcm";
        mimes[".mp3"] = "audio/mpeg3";
        mimes[".ogg"] = "audio/ogg";                  // Ogg Vorbis, Speex, Flac and other audio; Defined in RFC 5334
        mimes[".opus"] = "audio/opus";                // Opus audio
        mimes[".vorbis"] = "audio/vorbis";            // Vorbis encoded audio; Defined in RFC 5215
        mimes[".wav"] = "audio/x-wav";

        mimes[".atom"] = "application/atom+xml";      // Atom feeds
        mimes[".dart"] = "application/dart";          // Dart files [8]
        mimes[".ejs"] = "application/ecmascript";     // ECMAScript/JavaScript; Defined in RFC 4329 (equivalent to application/javascript but with stricter processing rules)
        mimes[".json"] = "application/json";          // JavaScript Object Notation JSON; Defined in RFC 4627
        mimes[".js"] = "application/javascript";      // ECMAScript/JavaScript; Defined in RFC 4329 (equivalent to application/ecmascript but with looser processing rules) It is not accepted in IE 8 or earlier - text/javascript is accepted but it is defined as obsolete in RFC 4329. The "type" attribute of the <script> tag in HTML5 is optional. In practice, omitting the media type of JavaScript programs is the most interoperable solution, since all browsers have always assumed the correct default even before HTML5.
        mimes[".bin"] = "application/octet-stream";   // Arbitrary binary data.[9] Generally speaking this type identifies files that are not associated with a specific application. Contrary to past assumptions by software packages such as Apache this is not a type that should be applied to unknown files. In such a case, a server or application should not indicate a content type, as it may be incorrect, but rather, should omit the type in order to allow the recipient to guess the type.[10]
        mimes[".pdf"] = "application/pdf";            // Portable Document Format, PDF has been in use for document exchange on the Internet since 1993; Defined in RFC 3778
        mimes[".ps"] = "application/postscript";      // PostScript; Defined in RFC 2046
        mimes[".rdf"] = "application/rdf+xml";        // Resource Description Framework; Defined by RFC 3870
        mimes[".rss"] = "application/rss+xml";        // RSS feeds
        mimes[".soap"] = "application/soap+xml";      // SOAP; Defined by RFC 3902
        mimes[".font"] = "application/font-woff";     // Web Open Font Format; (candidate recommendation; use application/x-font-woff until standard is official)
        mimes[".xhtml"] = "application/xhtml+xml";    // XHTML; Defined by RFC 3236
        mimes[".xml"] = "application/xml";            // XML files; Defined by RFC 3023
        mimes[".zip"] = "application/zip";            // ZIP archive files; Registered[11]
        mimes[".gz"] = "application/gzip";            // Gzip, Defined in RFC 6713
        mimes[".nacl"] = "application/x-nacl";        // Native Client web module (supplied via Google Web Store only)
        mimes[".pnacl"] = "application/x-pnacl";      // Portable Native Client web module (may supplied by any website as it is safer than x-nacl)
    };

    size_t ext = pathfile.find_last_of(".");
    if( ext != std::string::npos ) {
        std::map<std::string, std::string>::const_iterator found = mimes.find( &pathfile[ext] );
        if( found != mimes.end() ) {
            return found->second;
        }
    }
    return mimes[".txt"];
}

std::string mime( const std::string &filename_ext ) {
    return std::string("Content-Type: ") + tag(filename_ext) + "; charset=utf-8\r\n";
};

std::string interval_from_headers( const std::string &header, const std::string &text1, const std::string &text2 ) {
    size_t pos = header.find(text1);
    if( pos == std::string::npos ) {
        return std::string();
    }
    pos += text1.size();
    size_t pos2 = header.find(text2, pos);
    if( pos2 == std::string::npos ) {
        return header.substr(pos);
    } else {
        return header.substr(pos, pos2-pos);
    }
}

std::string extract_from_headers( const std::string &header, const std::string &text ) {
    size_t pos = header.find(text);
    if( pos == std::string::npos ) {
        return std::string();
    }
    pos += text.size();
    size_t pos2 = header.find( "\r\n", pos );
    if( pos2 == std::string::npos ) {
        return header.substr(pos);
    } else {
        return header.substr(pos, pos2-pos);
    }
}

struct daemon {
    int socket;
    const std::map< std::string /*path*/, route66::callback /*fn*/ > *routers;
    std::string base;

    daemon() : socket(-1), routers(0)
    {}

    void operator()() {
        bool shutdown = false;
        while( !shutdown ) {
            enum { buflen = 1024 * 1024, SPACES = 2 /*>=2*/ };

            int child = static_cast<int>(ACCEPT(socket,0,0)), o = 0, h[SPACES] = {}, hi = 0;
            if( child < 0 ) continue;

            std::string buf( buflen, '\0' );
            char *b = &buf[0];

            while(hi<SPACES&&o<buflen) {
                int n = READ(child,b+o,buflen-o);
                if(n<=0) { break; }
                else {
                    int i = o;
                    o+=n;
                    for(;i<n&&hi<SPACES;i++) {
                    if((hi < SPACES) && b[i] == ' ') { h[hi++] = i; }
                    }
                }
            }

            if(hi >= SPACES) {
                b[ o ] = '\0';
                buf.resize( o );

                const char *logmask = std::getenv("R66LOG");
                if( logmask ) {
                    if( matches(b, logmask) ) {
                        //__asm int 3;
                        //fprintf(stdout, "%s,%s\n", multipart.c_str(), boundary.c_str());
                    }
                }

                typedef std::map< std::string, std::string > filemap;
                static std::map< std::string /*boundary*/, filemap > boundaries;

                unsigned curlen = 0;

                std::string boundary = std::string("--") + extract_from_headers( buf, "Content-Type: multipart/form-data; boundary=");
                std::string eof = boundary + "--\r\n";
                std::string headers = buf.substr( 0, buf.find("\r\n\r\n") + 4 );
                std::string payload = buf.substr( headers.size() );

                while( boundary.size() > 2 && !endswith( payload, eof ) ) {
                    std::stringstream head1, head2;
                    head1 << "HTTP/1.1 100 Continue\r\n";
                    WRITE( child, head1.str().c_str(), head1.str().size() );

                    o = 0;
                    buf.clear();
                    buf.resize( buflen, '\0' );

                    int n;
                    n = READ(child,b+o,buflen-o);
                    if(n<=0) { break; }

                    buf.resize( n );

                    payload += buf;
                }

                route66::request rq;

                if( boundary.size() > 2 ) {
                    payload = interval_from_headers( payload, boundary + "\r\n", boundary );

                    {
                        std::string headers = payload.substr( 0, payload.find("\r\n\r\n") + 4 );
                        std::string file = extract_from_headers( headers, "Content-Disposition: form-data; name=");
                        file = file.substr( 1, file.size() - 2 ); // remove ""

                        payload = payload.substr( headers.size() );
                        rq.multipart[ file ] = payload;
                    }

                    buf = headers;
                }

                rq.data = buf;

                char org = b[h[1]];
                b[h[1]] = '\0';

#if 1
                socklen_t len;
                struct sockaddr_storage addr;
                char ipstr[INET_ADDRSTRLEN];
                len = sizeof addr;
                getpeername(child, (struct sockaddr*) &addr, &len);
                struct sockaddr_in *s = (struct sockaddr_in *) &addr;
                inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
                if( std::strcmp("127.0.0.1", ipstr) == 0 ) rq.ip = "localhost";
                else rq.ip = ipstr;

                struct sockaddr_in l;
                len = sizeof(l);
                if (getsockname(child, (struct sockaddr *)&l, &len) != -1) {
                    rq.port = ntohs(l.sin_port);
                } else {
                    rq.port = 0;
                }
#endif

                std::string method( b, h[0] );
                std::string url( &b[h[0]+1] ); //skip "GET "
                b[h[1]] = org;

                std::string path, options;

                size_t mark = url.find_first_of('?');
                if( mark == std::string::npos ) {
                    path = url;
                } else {
                    path = url.substr( 0, mark );
                    options = url.substr( mark + 1 );
                }

                if( method == "PUT" || method == "POST" )  {
                    options = &b[ std::string(b).find("\r\n\r\n") + 4 ];
                }

                std::string route = method + " " + path;
                std::map< std::string, route66::callback >::const_iterator found = routers->find( route );
                if( found == routers->end() ) {
                    // bad route, try index wildcard matching
                    for( found = routers->begin(); found != routers->end(); ++found ) {
                        if( matches( route, found->first ) ) {
                            break;
                        }
                    }
                }
                if( found == routers->end() ) {
                    // bad route, try index fallback
                    found = routers->find("GET /");
                }

                if( found != routers->end() ) {

                    rq.method = method;
                    rq.url = url;
                    rq.uri = path;
                    rq.options = options;

                    urldecode2( rq.uri );

                    // arguments
                    std::vector< std::string > split = tokenize( options, "&" );
                    for( std::vector< std::string >::const_iterator it = split.begin(), end = split.end(); it != end; ++it ) {
                        const std::string &glued = *it;
                        std::vector< std::string > pair = tokenize( glued, "=" );
                        if( 2 == pair.size() ) {
                            rq.arguments[ urldecode2(pair[0]) ] = urldecode2(pair[1]);
                        } else {
                            rq.arguments[ urldecode2(pair[0]) ] = std::string();
                        }
                    }

                    // normalize & rebase (if needed)
                    if( rq.uri == "" || rq.uri == "/" ) rq.uri = "index.html";
                    rq.uri = base + rq.uri;

                    if( logmask ) {
                        char addr[64]; sprintf(addr, "%s:%d", rq.ip.c_str(), rq.port );
                        if( matches(addr, logmask) || matches(rq.uri, logmask) || matches(rq.data, logmask) ) {
                            fprintf(stdout, "%s - %s\n%s\n", addr, rq.uri.c_str(), rq.str().c_str());
                        }
                    }

                    {
                        const route66::callback &fn = found->second;
                        std::stringstream headers, contents;
                        int httpcode = fn( rq, headers, contents );

                        std::string datas = contents.str();
                        shutdown = ( 0 == httpcode ? true : false );

                        if( !shutdown && datas.size() ) {
                            std::stringstream head1, head2;
                            head1 << "HTTP/1.1 " << httpcode << " OK\r\n";
                            head2 << headers.str() << "Content-Length: " << datas.size() << "\r\n\r\n";

                            WRITE( child, head1.str().c_str(), head1.str().size() );
                            WRITE( child, head2.str().c_str(), head2.str().size() );
                            if( method != "HEAD" )
                            WRITE( child, datas.c_str(), datas.size() );
                        }
                    }

                }

                SHUTDOWN(child);
                CLOSE(child);
            }
        }
    }
};

namespace {

    typedef std::map< std::string /*path*/, route66::callback /*fn */> routers;

    bool create( unsigned port, const std::string &mask, const route66::callback &fn, std::string base ) {
        static std::map< unsigned, routers > daemons;

        if( daemons.find(port) == daemons.end() ) {
            INIT();
            int socket = static_cast<int>(SOCKET(PF_INET, SOCK_STREAM, IPPROTO_IP));
            if( socket < 0 ) {
                return "cannot init socket", false;
            }

            struct sockaddr_in l;
            std::memset( &l, 0, sizeof(sockaddr_in) );
            l.sin_family = AF_INET;
            l.sin_port = htons(port);
            l.sin_addr.s_addr = INADDR_ANY;
    #       if !defined(_WIN32)
            int r = 1; setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,&r,sizeof(r));
    #       endif
            BIND(socket,(struct sockaddr *)&l,sizeof(l));
            LISTEN(socket,5);

            route66::daemon dm;
            dm.socket = socket;
            dm.routers = &daemons[ port ];
            dm.base = base;
            std::thread( dm ).detach();
        }

        daemons[ port ][ mask ] = fn;

        return "ok", true;
    }
}

bool create( unsigned port, const std::string &mask, const route66::callback &fn ) {
    return create( port, mask, fn, std::string() );
}

namespace {

    std::string get_app_folder() {
#       ifdef _WIN32
            char s_path[MAX_PATH] = {0};
            const size_t len = MAX_PATH;

            HMODULE mod = 0;
            ::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&get_app_folder, &mod);
            DWORD size = ::GetModuleFileNameA(mod, s_path, (DWORD)len);

            while( size > 0 ) {
                if( s_path[size] == '\\' ) {
                    s_path[size+1] = '\0';
                    return s_path;
                }
                --size;
            }
            return std::string();
#       else
            char arg1[20];
            char exepath[PATH_MAX + 1] = {0};

            sprintf( arg1, "/proc/%d/exe", getpid() );
            readlink( arg1, exepath, 1024 );
            return std::string( exepath );
#       endif
    }

    int file_reader( route66::request &request, std::ostream &headers, std::ostream &contents ) {
        std::ifstream ifs( request.uri.c_str(), std::ios::binary );
        if( ifs.good() ) {
            headers << route66::mime(request.uri);
            contents << ifs.rdbuf();
            return 200;
        } else {
            headers << route66::mime(".text");
            contents << "404: not found" << std::endl;
            //contents << request.uri << std::endl;
            return 404;
        }
    }
}

bool fserve( unsigned port, const std::string &mask, const std::string &path ) {
    return create( port, mask, file_reader, get_app_folder() + path );
}

#undef INIT
#undef SOCKET
#undef ACCEPT
#undef CONNECT
#undef CLOSE
#undef RECV
#undef READ
#undef SELECT
#undef SEND
#undef WRITE
#undef GETSOCKOPT
#undef SETSOCKOPT

#undef BIND
#undef LISTEN
#undef SHUTDOWN
#undef SHUTDOWN_R
#undef SHUTDOWN_W

/*/ 1XX informational
/*/ const httpcode SHUTDOWN = { 0, "Shutdown requested" };
/*/ 1XX informational
/*/ const httpcode CONTINUE = { 100, "The request can be continued." }; /*/
/*/ const httpcode SWITCH_PROTOCOLS = { 101, "The server has switched protocols in an upgrade header." };
/*/ 2XX success
/*/ const httpcode OK = { 200, "The request completed successfully." }; /*/
/*/ const httpcode CREATED = { 201, "The request has been fulfilled and resulted in the creation of a new resource." }; /*/
/*/ const httpcode ACCEPTED = { 202, "The request has been accepted for processing, but the processing has not been completed." }; /*/
/*/ const httpcode PARTIAL = { 203, "The returned meta information in the entity-header is not the definitive set available from the originating server." }; /*/
/*/ const httpcode NO_CONTENT = { 204, "The server has fulfilled the request, but there is no new information to send back." }; /*/
/*/ const httpcode RESET_CONTENT = { 205, "The request has been completed, and the client program should reset the document view that caused the request to be sent to allow the user to easily initiate another input action." }; /*/
/*/ const httpcode PARTIAL_CONTENT = { 206, "The server has fulfilled the partial GET request for the resource." }; /*/
/*/ const httpcode WEBDAV_MULTI_STATUS = { 207, "During a World Wide Web Distributed Authoring and Versioning (WebDAV) operation, this indicates multiple status const httpcodes for a single response. The response body contains Extensible Markup Language (XML) that describes the status const httpcodes. For more information, see HTTP Extensions for Distributed Authoring." };
/*/ 3XX redirection
/*/ const httpcode AMBIGUOUS = { 300, "The requested resource is available at one or more locations." }; /*/
/*/ const httpcode MOVED = { 301, "The requested resource has been assigned to a new permanent Uniform Resource Identifier (URI), and any future references to this resource should be done using one of the returned URIs." }; /*/
/*/ const httpcode REDIRECT = { 302, "The requested resource resides temporarily under a different URI." }; /*/
/*/ const httpcode REDIRECT_METHOD = { 303, "The response to the request can be found under a different URI and should be retrieved using a GET HTTP verb on that resource." }; /*/
/*/ const httpcode NOT_MODIFIED = { 304, "The requested resource has not been modified." }; /*/
/*/ const httpcode USE_PROXY = { 305, "The requested resource must be accessed through the proxy given by the location field." }; /*/
/*/ const httpcode REDIRECT_KEEP_VERB = { 307, "The redirected request keeps the same HTTP verb. HTTP/1.1 behavior." };
/*/ 4XX client's fault
/*/ const httpcode BAD_REQUEST = { 400, "The request could not be processed by the server due to invalid syntax." }; /*/
/*/ const httpcode DENIED = { 401, "The requested resource requires user authentication." }; /*/
/*/ const httpcode PAYMENT_REQ = { 402, "Not implemented in the HTTP protocol." }; /*/
/*/ const httpcode FORBIDDEN = { 403, "The server understood the request, but cannot fulfill it." }; /*/
/*/ const httpcode NOT_FOUND = { 404, "The server has not found anything that matches the requested URI." }; /*/
/*/ const httpcode BAD_METHOD = { 405, "The HTTP verb used is not allowed." }; /*/
/*/ const httpcode NONE_ACCEPTABLE = { 406, "No responses acceptable to the client were found." }; /*/
/*/ const httpcode PROXY_AUTH_REQ = { 407, "Proxy authentication required." }; /*/
/*/ const httpcode REQUEST_TIMEOUT = { 408, "The server timed out waiting for the request." }; /*/
/*/ const httpcode CONFLICT = { 409, "The request could not be completed due to a conflict with the current state of the resource. The user should resubmit with more information." }; /*/
/*/ const httpcode GONE = { 410, "The requested resource is no longer available at the server, and no forwarding address is known." }; /*/
/*/ const httpcode LENGTH_REQUIRED = { 411, "The server cannot accept the request without a defined content length." }; /*/
/*/ const httpcode PRECOND_FAILED = { 412, "The precondition given in one or more of the request header fields evaluated to false when it was tested on the server." }; /*/
/*/ const httpcode REQUEST_TOO_LARGE = { 413, "The server cannot process the request because the request entity is larger than the server is able to process." }; /*/
/*/ const httpcode URI_TOO_LONG = { 414, "The server cannot service the request because the request URI is longer than the server can interpret." }; /*/
/*/ const httpcode UNSUPPORTED_MEDIA = { 415, "The server cannot service the request because the entity of the request is in a format not supported by the requested resource for the requested method." }; /*/
/*/ const httpcode RETRY_WITH = { 449, "The request should be retried after doing the appropriate action." };
/*/ 5XX server's fault
/*/ const httpcode SERVER_ERROR = { 500, "The server encountered an unexpected condition that prevented it from fulfilling the request." }; /*/
/*/ const httpcode NOT_SUPPORTED = { 501, "The server does not support the functionality required to fulfill the request." }; /*/
/*/ const httpcode BAD_GATEWAY = { 502, "The server, while acting as a gateway or proxy, received an invalid response from the upstream server it accessed in attempting to fulfill the request." }; /*/
/*/ const httpcode SERVICE_UNAVAIL = { 503, "The service is temporarily overloaded." }; /*/
/*/ const httpcode GATEWAY_TIMEOUT = { 504, "The request was timed out waiting for a gateway." }; /*/
/*/ const httpcode VERSION_NOT_SUP = { 505, "The server does not support the HTTP protocol version that was used in the request message." };

}


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
            //$windows(
            //	std::string path = std::string( std::getenv("TEMP") ? std::getenv("TEMP") : "." ) + std::tmpnam(0);
            //	while( path.size() && path.at( path.size() - 1 ) == '.' ) path.resize( path.size() - 1 );
            //	return path;
            //)
            //$welse(
                return std::string() + std::tmpnam(0);
            //)
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

                    for( std::size_t i = 0, start = kTraceyStacktraceSkipBegin, end = callstack.frames.size() - 1 - kTraceyStacktraceSkipEnd; start+i <= end; ++i ) {

                        std::size_t size;
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
                            for( std::size_t i = 0, end = frames.size(); i < end; ++i ) {
                                translate[ frames[i] ] = tracey::string("\1", frames[i]);
                            }
                        } else {
                            for( std::size_t i = 0, end = frames.size(); i < end; ++i ) {
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
                        kTraceyfPrintf( fp, "%s", tracey::print(tree, static_cast<double>(wasted), translate, "{tabs}[{size}] ({value}) {key}\n", "{value}", true).c_str() );
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

void *operator new( size_t size )
$windows
(
    throw()
)
$welse
(
    throw(std::bad_alloc)
)
{
    return tracey::watch( tracey::malloc( size ), size );
}

void *operator new[]( size_t size )
$windows
(
    throw()
)
$welse
(
    throw(std::bad_alloc)
)
{
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
            $msvc(
                return _strdup( tracey::report().c_str() );
            )
            $welse(
                return strdup( tracey::report().c_str() );
            )
        }
        char *tracey_summary() {
            $msvc(
                return _strdup( tracey::summary().c_str() );
            )
            $welse(
                return strdup( tracey::summary().c_str() );
            )
        }
        char *tracey_version() {
            $msvc(
                return _strdup( tracey::version().c_str() );
            )
            $welse(
                return strdup( tracey::version().c_str() );
            )
        }
        char *tracey_url() {
            $msvc(
                return _strdup( tracey::url().c_str() );
            )
            $welse(
                return strdup( tracey::url().c_str() );
            )
        }
        char *tracey_settings() {
            $msvc(
                return _strdup( tracey::settings().c_str() );
            )
            $welse(
                return strdup( tracey::settings().c_str() );
            )
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

