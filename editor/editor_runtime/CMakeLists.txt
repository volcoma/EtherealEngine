file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable (editor ${libsrc})

target_link_libraries(editor PUBLIC editor_core)
target_link_libraries(editor PUBLIC runtime)

target_include_directories (editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(MINGW)
	set_target_properties(editor PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()


check_cxx_compiler_flag("-Wa,-mbig-obj" HAS_MBIG_OBJ)

if(HAS_MBIG_OBJ)
    target_compile_options(editor PUBLIC "-Wa,-mbig-obj")
elseif(MSVC)
    target_compile_options(editor PUBLIC "/bigobj")

endif()
