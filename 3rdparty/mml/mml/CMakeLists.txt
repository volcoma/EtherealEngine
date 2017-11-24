cmake_minimum_required(VERSION 2.8.3)

project(mml)

# include the configuration file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Macros.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake)

# add an option for choosing the build type (shared or static)
#if(NOT (MML_OS_IOS OR MML_OS_ANDROID))
#    mml_set_option(BUILD_SHARED_LIBS TRUE BOOL "TRUE to build MML as shared libraries, FALSE to build it as static libraries")
#else()
    if(MML_OS_IOS)
        set(BUILD_SHARED_LIBS FALSE)
#    elseif(MML_OS_ANDROID)
#        set(BUILD_SHARED_LIBS TRUE)
    endif()
#endif()

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

# add the MML header path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src/mml)