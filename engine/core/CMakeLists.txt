cmake_minimum_required(VERSION 3.7)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
endif()

include_directories(math reflection serialization)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (core ${libsrc})

target_link_libraries(core PUBLIC rttr_core_lib)
target_link_libraries(core PUBLIC cereal)
target_link_libraries(core PUBLIC glm_static)