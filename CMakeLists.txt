cmake_minimum_required(VERSION 3.0)

project (ethereal)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(macros)
include(modules/sanitizers_support)
include(modules/clang_tidy_support)
include(modules/add_ide_support)

set( BUILD_SHARED_LIBS OFF CACHE BOOL "Build package with shared libraries." FORCE)

list(APPEND SANITIZERS "custom")
#enable_sanitizers("${SANITIZERS}")

detect_platform()
print_info()
set_output_paths()

add_subdirectory_ex(3rdparty)

set_project_custom_defines()
#set_clang_tidy_args("-checks=-*,performance-*,modernize-*,-modernize-use-equals-default")
add_subdirectory_ex(engine)
add_subdirectory_ex(editor)
