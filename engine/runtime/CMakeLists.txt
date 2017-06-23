cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (runtime ${libsrc})

target_link_libraries(runtime PUBLIC core)
target_link_libraries(runtime PUBLIC mml-window)
