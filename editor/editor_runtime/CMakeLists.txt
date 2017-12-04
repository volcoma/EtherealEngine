file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable (editor ${libsrc})

target_link_libraries(editor PUBLIC editor_core)
target_link_libraries(editor PUBLIC runtime)
