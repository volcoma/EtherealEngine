add_subdirectory_ex(core)
add_subdirectory_ex(runtime)

# Make sure the compiler can find include files for our library
# when other libraries or executables link to it
target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
