file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (filesystem ${libsrc})

target_link_libraries(filesystem PUBLIC boost_filesystem)
