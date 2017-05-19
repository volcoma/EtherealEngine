cmake_minimum_required(VERSION 3.7)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc)

list(REMOVE_ITEM libsrc ${CMAKE_CURRENT_SOURCE_DIR}/src/amalgamated.cpp)

add_library (bgfx STATIC ${libsrc})

target_link_libraries(bgfx PUBLIC bimg)

if(MSVC)
	set_target_properties(bgfx PROPERTIES LINK_FLAGS "/ignore:4221")
	target_compile_definitions(bgfx PUBLIC _CRT_SECURE_NO_WARNINGS=1)
endif()
