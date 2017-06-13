cmake_minimum_required(VERSION 3.7)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (logging ${libsrc})

target_link_libraries(logging PUBLIC spdlog)
