project(mml)

# include the configuration file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Macros.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake)

# CMake's built-in Android support requires 3.7.0
if(CMAKE_SYSTEM_NAME MATCHES "Android")
    cmake_minimum_required(VERSION 3.7.2)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

# these options have to be set before CMake detects/configures the toolchain

# Suppress Cygwin legacy warning
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

# Suppress Mac OS X RPATH warnings and adopt new related behaviors
cmake_policy(SET CMP0042 NEW)
if (NOT CMAKE_VERSION VERSION_LESS 3.9)
    cmake_policy(SET CMP0068 NEW)
endif()

# add some default value for some additional macOS variable
# note that those variables are ignored on other systems
if(NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "macOS architecture to build; 64-bit is expected" FORCE)
endif()
if(NOT CMAKE_OSX_SYSROOT)
    # query the path to the default SDK, will fail on non-macOS, but it's okay.
    execute_process(COMMAND xcodebuild -sdk macosx -version Path
                    COMMAND head -n 1
                    COMMAND tr -d '\n'
                    OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
                    ERROR_QUIET)
endif()

# add an option for choosing the build type (shared or static)
if(NOT (MML_OS_IOS OR MML_OS_ANDROID))
    mml_set_option(BUILD_SHARED_LIBS TRUE BOOL "TRUE to build MML as shared libraries, FALSE to build it as static libraries")
else()
    if(MML_OS_IOS)
        set(BUILD_SHARED_LIBS FALSE)
    elseif(MML_OS_ANDROID)
        set(BUILD_SHARED_LIBS TRUE)
    endif()
endif()

mml_set_option(MML_BUILD_WINDOW TRUE BOOL "TRUE to build mml's Window module.")
# Visual C++: remove warnings regarding SL security and algorithms on pointers
if(MML_COMPILER_MSVC)
    # add an option to choose whether PDB debug symbols should be generated (defaults to true when possible)
    if(CMAKE_VERSION VERSION_LESS 3.1)
        mml_set_option(MML_GENERATE_PDB FALSE BOOL "True to generate PDB debug symbols, FALSE otherwise. Requires CMake 3.1.")
        if(MML_GENERATE_PDB)
            message(FATAL_ERROR "Generation of PDB files (MML_GENERATE_PDB) requires at least CMake 3.1.0")
        endif()
    else()
        mml_set_option(MML_GENERATE_PDB TRUE BOOL "True to generate PDB debug symbols, FALSE otherwise. Requires CMake 3.1.")
    endif()

    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
endif()


# define an option for choosing between static and dynamic C runtime (Windows only)

if(MML_OS_WINDOWS)
    mml_set_option(MML_USE_STATIC_STD_LIBS FALSE BOOL "TRUE to statically link to the standard libraries, FALSE to use them as DLLs")
    # the following combination of flags is not valid
    if (BUILD_SHARED_LIBS AND MML_USE_STATIC_STD_LIBS)
        message(FATAL_ERROR "BUILD_SHARED_LIBS and MML_USE_STATIC_STD_LIBS cannot be used together")
    endif()

    # for VC++, we can apply it globally by modifying the compiler flags
    if(MML_COMPILER_MSVC AND MML_USE_STATIC_STD_LIBS)
        foreach(flag
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
        endforeach()
    endif()
endif()

# add the MML header path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src/mml)
