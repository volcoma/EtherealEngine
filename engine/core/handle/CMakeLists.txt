file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (handle ${libsrc})
