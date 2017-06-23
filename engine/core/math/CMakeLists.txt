file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (math ${libsrc})

target_link_libraries(math PUBLIC glm_static)
