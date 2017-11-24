file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc *.m)

if(NOT WIN32)
	list(REMOVE_ITEM libsrc ${CMAKE_CURRENT_SOURCE_DIR}/nfd/nfd_win.cpp)
endif()
	
if(NOT UNIX)
	list(REMOVE_ITEM libsrc ${CMAKE_CURRENT_SOURCE_DIR}/nfd/nfd_gtk.c)
endif()

if(NOT APPLE)
	list(REMOVE_ITEM libsrc ${CMAKE_CURRENT_SOURCE_DIR}/nfd/nfd_cocoa.m)
endif()

if(UNIX)
    # Use the package PkgConfig to detect GTK+ headers/library files
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    # Setup CMake to use GTK+, tell the compiler where to look for headers
    # and to the linker where to look for libraries
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
endif()

add_library (filedialog ${libsrc})

if(UNIX)
    target_link_libraries(filedialog ${GTK3_LIBRARIES})
endif()


if(MSVC)
set(filedialog_flags
_CRT_SECURE_NO_WARNINGS
_CRT_SECURE_NO_DEPRECATE)
endif()

target_compile_definitions(filedialog PUBLIC ${filedialog_flags})
