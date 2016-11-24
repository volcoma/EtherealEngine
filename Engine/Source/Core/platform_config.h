#pragma  once

#if (__cplusplus < 201103L && !defined(_MSC_VER)) || (defined(_MSC_VER) && (_MSC_VER < 1700)) || (defined(__GLIBCXX__) && __GLIBCXX__ < 20130322L)
#define $cpp11          $no
#define $cpp03          $yes
#else
#define $cpp11          $yes
#define $cpp03          $no
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

#ifdef __ANDROID__
#   define $android   $yes
#   define $aelse     $no
#else
#   define $android   $no
#   define $aelse     $yes
#endif

// Compiler utils

#if ULONG_MAX == 4294967295
#   define $bits64    $yes
#   define $bits32    $no
#else
#   define $bits64    $no
#   define $bits32    $yes
#endif

#if defined(DEBUG) || defined(_DEBUG) || defined(NDEBUG) || defined(_NDEBUG)
#   define $release   $no
#   define $debug     $yes
#else
#   define $release   $yes
#   define $debug     $no
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

#if $on($windows) || $on($linux) || $on($apple) || $on($android)
#   define $undefined_os $no
#else
#   define $undefined_os $yes
#endif

template<bool>
inline bool eval()
{
	return true;
}

template<>
inline bool eval<false>()
{
	return false;
}
#define runtime_eval(_x) eval<!!(_x)>()
