add_subdirectory_ex(editor_core)
add_subdirectory_ex(editor_runtime)

# Make sure the compiler can find include files for our library
# when other libraries or executables link to it
target_include_directories (editor_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
