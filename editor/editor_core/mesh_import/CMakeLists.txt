file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (mesh_import ${libsrc})

target_link_libraries(mesh_import PUBLIC assimp)
