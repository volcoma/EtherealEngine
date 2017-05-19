cmake_minimum_required(VERSION 3.7)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc)

list(REMOVE_ITEM libsrc ${CMAKE_CURRENT_SOURCE_DIR}/src/amalgamated.cpp)

add_library (bx STATIC ${libsrc})

if(MSVC)
	set_target_properties(bx PROPERTIES LINK_FLAGS "/ignore:4221")
	target_compile_definitions(bx PUBLIC _CRT_SECURE_NO_WARNINGS=1)
endif()
if(WIN32)
	target_link_libraries(bx PUBLIC psapi)
endif()