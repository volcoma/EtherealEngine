cmake_minimum_required(VERSION 3.7)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc)

add_library (bimg STATIC ${libsrc})

target_link_libraries(bimg PUBLIC edtaa3)
target_link_libraries(bimg PUBLIC etc1)
target_link_libraries(bimg PUBLIC etc2)
target_link_libraries(bimg PUBLIC iqa)
target_link_libraries(bimg PUBLIC libsquish)
target_link_libraries(bimg PUBLIC lodepng)
target_link_libraries(bimg PUBLIC nvtt)
target_link_libraries(bimg PUBLIC pvrtc)
target_link_libraries(bimg PUBLIC bx)

if(MSVC)
	set_target_properties(bimg PROPERTIES LINK_FLAGS "/ignore:4221")
	target_compile_definitions(bimg PUBLIC _CRT_SECURE_NO_WARNINGS=1)
endif()