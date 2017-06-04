cmake_minimum_required(VERSION 3.7)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable(editor ${libsrc})

target_link_libraries(editor PUBLIC runtime)
target_link_libraries(editor PUBLIC filedialog)
target_link_libraries(editor PUBLIC assimp)
target_link_libraries(editor PUBLIC shaderc) 
target_link_libraries(editor PUBLIC texturec)

install(TARGETS editor
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
        LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib
        ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/lib/static)
