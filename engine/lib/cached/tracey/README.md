tracey :squirrel: <a href="https://travis-ci.org/r-lyeh/tracey"><img src="https://api.travis-ci.org/r-lyeh/tracey.svg?branch=master" align="right" /></a>
======

- Tracey is a C++ static library that reports memory leaks and provides precise stacktraces with no false positives.
- Tracey is simple. Just link it.
- Tracey is easy to integrate. No source modification is required.
- Tracey is clean. Macroless and callstack based. No new/delete macro hacks.
- Tracey is lazy. Should work out of the box. Plenty configuration options are provided anyways.
- Tracey is tiny. One header and one source file.
- Tracey is handy. Generates HTML logs that can be collapsed per branch in [Sublime Text 2](http://sublimetext.com) and related editors.
- Tracey is cross-platform. Builds on Windows/Linux/MacosX. Compiles on g++/clang/msvc.
- Tracey is stand-alone. OS dependencies only. No third party dependencies.
- Tracey is zlib/libpng licensed.

### cons
- No hooks for `malloc()`/`free()`. Tracey supports `new`/`delete` C++ memory operators only (atm).
- Slower runtime speed than regular builds (but not that much). There is room for improvement though.

### sample
```c++
// tracey is callstack based. no dirty new/delete macro tricks.
// tracey is a static library. requires no source modification. just link it.

static int *static_cpp_leak = new int;

int main() {
    new int [400];
}
```

### special notes
- g++ users: both `-std=c++0x` and `-lpthread` may be required when compiling `tracey.cpp`

### Possible outputs (msvc/g++/clang)
```
C:\tracey> rem LINUX: g++ sample.cc tracey.cpp -g -lpthread -std=c++0x
C:\tracey> rem APPLE: clang++ sample.cc tracey.cpp -g -o sample -std=c++11 -stdlib=libc++
C:\tracey> rem WINDOWS following
C:\tracey> cl sample.cc tracey.cpp /Zi
C:\tracey> sample
<tracey/tracey.cpp> says: tracey-0.22.c ready
<tracey/tracey.cpp> says: using realloc (f:\dd\vctools\crt\crtw32\heap\realloc.c, line 62) as realloc
<tracey/tracey.cpp> says: using memset (f:\dd\vctools\crt\crtw32\string\i386\memset.asm, line 61) as memset
<tracey/tracey.cpp> says: using printf (f:\dd\vctools\crt\crtw32\stdio\printf.c, line 49) as printf
<tracey/tracey.cpp> says: using FatalExit as exit
<tracey/tracey.cpp> says: using fopen (f:\dd\vctools\crt\crtw32\stdio\fopen.c, line 124) as fopen
<tracey/tracey.cpp> says: using fclose (f:\dd\vctools\crt\crtw32\stdio\fclose.c, line 43) as fclose
<tracey/tracey.cpp> says: using fprintf (f:\dd\vctools\crt\crtw32\stdio\fprintf.c, line 49) as fprintf
<tracey/tracey.cpp> says: with C++ exceptions=enabled
<tracey/tracey.cpp> says: with kTraceyBudgetOverhead=101.000000%
<tracey/tracey.cpp> says: with kTraceyMaxStacktraces=128 range[0..0]
<tracey/tracey.cpp> says: with kTraceyReportWildPointers=no
<tracey/tracey.cpp> says: with kTraceyDefineMemoryOperators=yes
<tracey/tracey.cpp> says: with kTraceyMemsetAllocations=yes
<tracey/tracey.cpp> says: with kTraceyStacktraceSkipBegin=0
<tracey/tracey.cpp> says: with kTraceyStacktraceSkipEnd=0
<tracey/tracey.cpp> says: with kTraceyReportOnExit=yes
<tracey/tracey.cpp> says: with kTraceyWebserverPort=2001
<tracey/tracey.cpp> says: with kTraceyHookLegacyCRT=0
<tracey/tracey.cpp> says: with kTraceyEnabled=1
<tracey/tracey.cpp> says: summary: highest peak: 1600 bytes total, 1600 bytes greatest peak // -8 allocs in use: 1472 bytes + overhead: 3 GB = total: 948 bytes
<tracey/tracey.cpp> says: creating report: C:\Users\MARIO~1.ROD\AppData\Local\Temp\san4xxx-tracey.html
<tracey/tracey.cpp> says: filtering leaks...
<tracey/tracey.cpp> says: found 2 leaks wasting 1604 bytes
<tracey/tracey.cpp> says: creating trees of frames...
<tracey/tracey.cpp> says: resolving 12 unique frames...
<tracey/tracey.cpp> says: converting tree of frames into tree of symbols...
<tracey/tracey.cpp> says: flattering tree of symbols...
C:\tracey>
```

and then in `C:\Users\MARIO~1.ROD\AppData\Local\Temp\san4xxx-tracey.html` log file...
```
<tracey/tracey.cpp> says: generated with tracey-0.22.c (https://github.com/r-lyeh/tracey)
<tracey/tracey.cpp> says: best viewed on foldable text editor (like SublimeText2) with tabs=2sp and no word-wrap
<tracey/tracey.cpp> says: error, 2 leaks found; 1604 bytes wasted ('lame' score)
<tracey/tracey.cpp> says: summary: highest peak: 1600 bytes total, 1600 bytes greatest peak // -8 allocs in use: 1472 bytes + overhead: 3 GB = total: 948 bytes
<tracey/tracey.cpp> says: report filename: C:\Users\MARIO~1.ROD\AppData\Local\Temp\san4xxx-tracey.html
[2] (100% .. 1604 bytes) begin
  [1] (100% .. 1604 bytes) RtlInitializeExceptionChain
    [1] (100% .. 1604 bytes) RtlInitializeExceptionChain
      [1] (100% .. 1604 bytes) BaseThreadInitThunk
        [2] (099% .. 1600 bytes) __tmainCRTStartup (f:\dd\vctools\crt\crtw32\startup\crt0.c, line 255)
          [1] (099% .. 1600 bytes) main (C:\tracey\sample.cc, line 7)
            [1] (099% .. 1600 bytes) operator new (C:\tracey\tracey.cpp, line 3319)
              [1] (099% .. 1600 bytes) tracey::watch (C:\tracey\tracey.cpp, line 3162)
                [1] (099% .. 1600 bytes) tracey::`anonymous namespace'::tracer (C:\tracey\tracey.cpp, line 3139)
        [2] (000% .. 4 bytes) __tmainCRTStartup (f:\dd\vctools\crt\crtw32\startup\crt0.c, line 237)
          [1] (000% .. 4 bytes) _cinit (f:\dd\vctools\crt\crtw32\startup\crt0dat.c, line 321)
            [1] (000% .. 4 bytes) _initterm (f:\dd\vctools\crt\crtw32\startup\crt0dat.c, line 954)
              [1] (000% .. 4 bytes) `dynamic initializer for 'static_cpp_leak'' (C:\tracey\sample.cc, line 4)
                [1] (000% .. 4 bytes) operator new (C:\tracey\tracey.cpp, line 3319)
                  [1] (000% .. 4 bytes) tracey::watch (C:\tracey\tracey.cpp, line 3162)
                    [1] (000% .. 4 bytes) tracey::`anonymous namespace'::tracer (C:\tracey\tracey.cpp, line 3139)
[2] (100% .. 1604 bytes) end
  [1] (100% .. 1604 bytes) tracey::`anonymous namespace'::tracer (C:\tracey\tracey.cpp, line 3139)
    [1] (100% .. 1604 bytes) tracey::watch (C:\tracey\tracey.cpp, line 3162)
      [1] (100% .. 1604 bytes) operator new (C:\tracey\tracey.cpp, line 3319)
        [2] (099% .. 1600 bytes) main (C:\tracey\sample.cc, line 7)
          [1] (099% .. 1600 bytes) __tmainCRTStartup (f:\dd\vctools\crt\crtw32\startup\crt0.c, line 255)
            [1] (099% .. 1600 bytes) BaseThreadInitThunk
              [1] (099% .. 1600 bytes) RtlInitializeExceptionChain
                [1] (099% .. 1600 bytes) RtlInitializeExceptionChain
        [2] (000% .. 4 bytes) `dynamic initializer for 'static_cpp_leak'' (C:\tracey\sample.cc, line 4)
          [1] (000% .. 4 bytes) _initterm (f:\dd\vctools\crt\crtw32\startup\crt0dat.c, line 954)
            [1] (000% .. 4 bytes) _cinit (f:\dd\vctools\crt\crtw32\startup\crt0dat.c, line 321)
              [1] (000% .. 4 bytes) __tmainCRTStartup (f:\dd\vctools\crt\crtw32\startup\crt0.c, line 237)
                [1] (000% .. 4 bytes) BaseThreadInitThunk
                  [1] (000% .. 4 bytes) RtlInitializeExceptionChain
                    [1] (000% .. 4 bytes) RtlInitializeExceptionChain
```

### API implementation (optional)
Backend implementation in `tracey.hpp`. Tweak these if needed.
```c++
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
```

### API configuration (optional)
Backend options in `tracey.hpp`. Tweak these until happy.
```c++
/*/ Tracey uses this value to simulate increased memory simulations (should be >= 1.0)
/*/ #define kTraceyBudgetOverhead          1.0
/*/ Tracey retrieves up to 128 traces by default. The longer the slower, though.
/*/ #define kTraceyMaxStacktraces          128
/*/ Tracey head position on every stacktrace. It does not skip backtraces by default.
/*/ #define kTraceyStacktraceSkipBegin     0 // $windows(4) $welse(0)
/*/ Tracey tail position on every stacktrace. It does not skip backtraces by default.
/*/ #define kTraceyStacktraceSkipEnd       0 // $windows(4) $welse(0)
/*/ Tracey linefeed character when logging.
/*/ #define kTraceyCharLinefeed            "\n"
/*/ Tracey tab character when logging.
/*/ #define kTraceyCharTab                 "\t"
/*/ When enabled, Tracey warns about deallocations on pointers that have been not allocated by Tracey (wild pointers)
/*/ #define kTraceyReportWildPointers      0
/*/ When enabled, Tracey warns about double allocations on same pointer
/*/ #define kTraceyReportDoubleAllocations 0
/*/ When enabled, Tracey memsets memory on mallocs()
/*/ #define kTraceyMemsetAllocations       1
/*/ When enabled, Tracey shows a report automatically on application exit.
/*/ #define kTraceyReportOnExit            1
/*/ When enabled, Tracey is enabled
/*/ #define kTraceyEnabled                 1
/*/ When >0, Tracey web service is enabled. Note: requires C++11
/*/ #define kTraceyWebserverPort           2001
/*/ When enabled, Tracey will hook up C CRT as well (@todo)
/*/ #define kTraceyHookLegacyCRT           0
/*/ When >0, Tracey will ignore branches of leaks that are smaller than given percentage. It does not ignore branches by default.
/*/ #define kTraceyTruncateBranchesSmallerThan 0.0 // 5.0%
/*/ When enabled, Tracey implements all new/delete operators; else user must use runtime API manually (see below).
/*/ #define kTraceyDefineMemoryOperators       1
```

### API C++ runtime (optional)
- `tracey::watch(ptr,size)` tells Tracey to watch a memory address.
- `tracey::forget(ptr)` tells Tracey to forget about a memory address.
- `tracey::clear()` tells Tracey to forget whole execution.
- `tracey::report()` creates a report and returns its physical address.
- `tracey::view(log)` views given report log.
- `tracey::badalloc()` throws a bad_alloc() exception, if possible.
- `tracey::fail(msg)` shows given error then fail.
- `tracey::nop()` does allocate/watch/forget/free some memory.
- `tracey::summary()` returns a brief summary.
- `tracey::version()` returns current version.
- `tracey::url()` returns project repository.
- `tracey::settings()` returns current settings.
- `tracey::scope()` raii scope monitoring.

### API C runtime (optional)
- `tracey_watch(ptr,size)` tells Tracey to watch a memory address.
- `tracey_forget(ptr)` tells Tracey to forget about a memory address.
- `tracey_clear()` tells Tracey to forget whole execution.
- `tracey_report()` creates a report and returns its physical address.
- `tracey_view(log)` views given report log.
- `tracey_badalloc()` throws a bad_alloc() exception, if possible.
- `tracey_fail(msg)` shows given error then fail.
- `tracey_nop()` does allocate/watch/forget/free some memory.
- `tracey_summary()` returns a brief summary.
- `tracey_version()` returns current version.
- `tracey_url()` returns project repository.
- `tracey_settings()` returns current settings.
- `tracey_view_report()` make report & view.

### API C runtime (optional)
- @todoc

### Changelog
- v1.0.0 (2015/11/04) 
  - Initial semantic versioning adherence
  - Fix 64-bit
- 0.24-c
  - Upgraded submodules
- 0.23-c
  - Webserver support (w/ C++03)
- 0.22-c
  - Source code clean up.
  - Faster implementation.
  - Better (more) documentation.
  - Safer behavior on any /MT /MTd /MD /MDd combination (MSVC).
  - Deprecated tinythread mutexes in favor of boost (only in C++03).
  - Simplified mutex locking. Requires compiler with thread-local storage support.
  - Amalgamated distribution made of code from different sublibraries: [heal](https://github.com/r-lyeh/heal), [oak](https://github.com/r-lyeh/oak), [route66](https://github.com/r-lyeh/route66)
  - Bugfixed wrong branch size on reports.
- 0.21-b
  - Tracey requires less memory now.
  - Memory usage shown on tree reports now.
- 0.20-b
  - Tracey behaves better in many aspects now.
  - Support for /MT and /MTd on Windows.
  - iOS support.
  - A more compatible symbol demangling on MacOs/iOS.
  - Faster report generation.
  - New smaller hierarchical tree reports that provide also more information than before.
  - Bugfixed symbol retrieval when other memory managers are present.
  - Improved symbol retrieval on windows.
  - Experimental webserver (to be improved).
  - New compiler tweaks.
  - Many new options and settings (unwinding stack size, truncation, etc...)
  - Bugfixed recursive deadlocks on tinythread.
  - A bunch of other minor bugfixes and tweaks.
  - Tracey throws exceptions only if they are enabled on your compiler now.
  - API upgraded.
  - Many things I have forgotten.
