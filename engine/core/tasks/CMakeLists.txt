file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (tasks ${libsrc})

target_link_libraries(tasks PUBLIC common_lib)
	
