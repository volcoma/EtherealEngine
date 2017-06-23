file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (serialization ${libsrc})

target_link_libraries(serialization PUBLIC cereal)
