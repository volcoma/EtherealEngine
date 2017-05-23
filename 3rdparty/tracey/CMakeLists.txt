cmake_minimum_required(VERSION 3.7)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc *.m)

add_library (tracey ${libsrc})

if(MSVC)
set(tracey_flags
_CRT_SECURE_NO_WARNINGS
_CRT_SECURE_NO_DEPRECATE)
endif()

target_compile_definitions(tracey PUBLIC ${tracey_flags})