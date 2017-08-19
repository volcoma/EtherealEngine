cmake_minimum_required(VERSION 3.0)

project (ethereal)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(macros)
include(modules/ECMEnableSanitizers)

option(BUILD_SHARED_LIBS
  "Build package with shared libraries."
  OFF
)
list(APPEND SANITIZERS "custom")
#enable_sanitizers("${SANITIZERS}")
detect_platform()
print_info()
set_output_paths()

add_subdirectory_ex(3rdparty)
add_subdirectory_ex(engine)

set_project_custom_defines()
add_subdirectory_ex(editor)
