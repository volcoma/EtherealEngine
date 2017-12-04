file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (gui ${libsrc})

target_link_libraries(gui PUBLIC imgui)
