/* Tracey, a simple and lightweight memory leak detector
 * Copyright (c) 2011,2012,2013,2014 r-lyeh
 * ZLIB/LibPNG licensed.
 */

#ifndef TRACEY_H
#define TRACEY_H

#define TRACEY_VERSION "1.0.0" /* (2015/11/04) Initial semantic versioning adherence; Fix 64-bit */

/*/ Remove this asterisk to highlight API --> */

/*/ Backend options. Tweak these until happy.
/*/

/*/ Tracey uses this value to simulate increased memory simulations (should be >= 1.0)
/*/ #define kTraceyBudgetOverhead              1.0
/*/ Tracey retrieves up to 128 traces by default. The longer the slower, though.
/*/ #define kTraceyMaxStacktraces              128
/*/ Tracey head position on every stacktrace. It does not skip backtraces by default.
/*/ #define kTraceyStacktraceSkipBegin         0
/*/ Tracey tail position on every stacktrace. It does not skip backtraces by default.
/*/ #define kTraceyStacktraceSkipEnd           0
/*/ Tracey linefeed character when logging.
/*/ #define kTraceyCharLinefeed                "\n"
/*/ Tracey tab character when logging.
/*/ #define kTraceyCharTab                     "\t"
/*/ When enabled, Tracey warns about deallocations on pointers that have been not allocated by Tracey (wild pointers)
/*/ #define kTraceyReportWildPointers          0
/*/ When enabled, Tracey warns about double allocations on same pointer
/*/ #define kTraceyReportDoubleAllocations     0
/*/ When enabled, Tracey memsets memory on mallocs()
/*/ #define kTraceyMemsetAllocations           1
/*/ When enabled, Tracey shows a report automatically on application exit.
/*/ #define kTraceyReportOnExit                $is($debug)
/*/ When enabled, Tracey is enabled
/*/ #define kTraceyEnabled                     $is($debug)
/*/ When >0, Tracey web service is enabled. Note: requires C++11
/*/ #define kTraceyWebserverPort               2001
/*/ When enabled, Tracey will hook up C CRT as well (@todo)
/*/ #define kTraceyHookLegacyCRT               0
/*/ When >0.0 (%), Tracey will ignore branches of leaks that are smaller than given percentage. It does not ignore branches by default.
/*/ #define kTraceyTruncateBranchesSmallerThan 0.0 
/*/ When enabled, Tracey implements all new/delete operators; else user must use runtime API manually (see below).
/*/ #define kTraceyDefineMemoryOperators       1

/*/ Backend implementation. Tweak these if needed.
/*/

/*/ Default report logger
/*/ #define kTraceyPrintf           std::printf
/*/ Default memory allocator
/*/ #define kTraceyRealloc          std::realloc
/*/ Default memory setter
/*/ #define kTraceyMemset           std::memset
/*/ Default filling character
/*/ #define kTraceyMemsetChar       '\0'
/*/ All fatal exits converge to this symbol
/*/ #define kTraceyDie              $windows(FatalExit) $welse( $cpp11(std::quick_exit) $cpp03(std::exit) )
/*/ All out-of-memory runtime asserts converge to this symbol.
/*/ #define kTraceyAssert           assert
/*/ All out-of-memory runtime exceptions converge to this sysmbol.
/*/ #define kTraceyBadAlloc         std::bad_alloc
/*/ Default wrapper to std::FILE
/*/ #define kTraceyfFile            std::FILE
/*/ Default wrapper to fopen()
/*/ #define kTraceyfOpen            std::fopen
/*/ Default wrapper to fclose()
/*/ #define kTraceyfClose           std::fclose
/*/ Default wrapper to fprintf()
/*/ #define kTraceyfPrintf          std::fprintf

/*/ size_t header (stddef.h fails on ArchLinux w/ clang 3.4)
/*/ 
#include <stdio.h>

#ifdef __cplusplus

#include <string>

namespace tracey {

    /*/ Basic API
    /*/
    void *watch( const void *ptr, size_t size );
    void *forget( const void *ptr );
    void  enable();
    void  disable();
    void  clear();

    /*/ Report API
    /*/
    std::string report();
    void view( const std::string &report );

    /*/ Unchecked memory API
    /*/
    void *realloc( void *ptr, size_t resize );
    void *malloc( size_t size );
    void *calloc( size_t num, size_t size );
    void *free( void *ptr );
    void *amalloc( size_t size, size_t alignment );

    /*/ Information API
    /*/
    std::string summary();
    std::string version();
    std::string url();
    std::string settings();

    /*/ Crash API
    /*/
    void fail( const char *message );
    void badalloc();

    /*/ RAII based monitoring API
    /*/
    struct scope {
         scope();
        ~scope();
        private: scope( const scope & );
        private: scope& operator=( const scope & ) const;
    };
}

extern "C" {
#endif

    /*/ Basic API 
    /*/
    void *tracey_watch( const void *ptr, size_t size );
    void *tracey_forget( const void *ptr );
    void  tracey_enable();
    void  tracey_disable();
    void  tracey_clear();

    /*/ Information API   -----  [!!] free() return value after use [!!]
    /*/
    char *tracey_report();
    char *tracey_settings();
    char *tracey_summary();    
    char *tracey_url();
    char *tracey_version();

    /*/ Report API 
    /*/
    void  tracey_view( const char *const report );
    void  tracey_view_report();

    /*/ Checked memory API 
    /*/
    void *tracey_checked_realloc( void *ptr, size_t resize );
    void *tracey_checked_malloc( size_t size );
    void *tracey_checked_calloc( size_t num, size_t size );
    void  tracey_checked_free( void *ptr );
    void *tracey_checked_amalloc( size_t size, size_t alignment );

    /*/ Unchecked memory API 
    /*/
    void *tracey_unchecked_realloc( void *ptr, size_t resize );
    void *tracey_unchecked_malloc( size_t size );
    void *tracey_unchecked_calloc( size_t num, size_t size );
    void  tracey_unchecked_free( void *ptr );
    void *tracey_unchecked_amalloc( size_t size, size_t alignment );

    /*/ Crash API 
    /*/
    void tracey_fail( const char *message );
    void tracey_badalloc();

#ifdef __cplusplus
}
#endif

#endif /* TRACEY_H */
