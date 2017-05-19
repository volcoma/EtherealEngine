cmake_minimum_required(VERSION 3.7)

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

add_library (filedialog ${libsrc})

if(MSVC)
set(filedialog_flags
_CRT_SECURE_NO_WARNINGS
_CRT_SECURE_NO_DEPRECATE)
endif()

target_compile_definitions(filedialog PUBLIC ${filedialog_flags})