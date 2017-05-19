cmake_minimum_required(VERSION 3.7)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable(editor ${libsrc})

target_link_libraries(editor PUBLIC runtime)
target_link_libraries(editor PUBLIC filedialog)
target_link_libraries(editor PUBLIC assimp)
target_link_libraries(editor PUBLIC SPIRV)
target_link_libraries(editor PUBLIC SPVRemapper)
target_link_libraries(editor PUBLIC glsl_optimizer)
target_link_libraries(editor PUBLIC fcpp)

install(TARGETS editor
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)