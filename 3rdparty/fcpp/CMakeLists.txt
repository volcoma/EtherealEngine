cmake_minimum_required(VERSION 3.7)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc)

list(REMOVE_ITEM libsrc ${CMAKE_CURRENT_SOURCE_DIR}/usecpp.c)

add_library (fcpp ${libsrc})

if(MSVC)
set(fcpp_flags
_CRT_SECURE_NO_WARNINGS
_CRT_SECURE_NO_DEPRECATE)
endif()

target_compile_definitions(fcpp PUBLIC ${fcpp_flags})