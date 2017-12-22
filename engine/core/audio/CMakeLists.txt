file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (audio ${libsrc})

target_link_libraries(audio PUBLIC OpenAL)
target_link_libraries(audio PUBLIC stb_vorbis)
