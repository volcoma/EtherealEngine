file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (graphics ${libsrc})

target_link_libraries(graphics PUBLIC bgfx)
