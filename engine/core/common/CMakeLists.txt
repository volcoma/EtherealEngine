add_library(common_lib INTERFACE)
target_add_ide_support(common_lib)

if(MINGW OR MSVC)
	target_link_libraries(common_lib INTERFACE wsock32)
endif()
