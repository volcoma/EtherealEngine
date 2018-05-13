file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (runtime ${libsrc})

target_link_libraries(runtime PUBLIC core)
target_link_libraries(runtime PUBLIC mml-window)

target_include_directories (runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
