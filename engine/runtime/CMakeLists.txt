cmake_minimum_required(VERSION 3.7)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (runtime ${libsrc})

target_link_libraries(runtime PUBLIC core)
target_link_libraries(runtime PUBLIC graphics)
#target_link_libraries(runtime PUBLIC sfml-window)
if(WIN32)
	target_link_libraries(runtime PUBLIC winmm)
endif()
