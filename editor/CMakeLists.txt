file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable(editor ${libsrc})

target_link_libraries(editor PUBLIC runtime)
target_link_libraries(editor PUBLIC filedialog)
target_link_libraries(editor PUBLIC assimp)
target_link_libraries(editor PUBLIC imgui)
target_link_libraries(editor PUBLIC shaderc) 
target_link_libraries(editor PUBLIC texturec)
