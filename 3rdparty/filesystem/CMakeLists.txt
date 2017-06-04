cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(filesystem LANGUAGES C CXX)

function(PrintSourceFiles)
  file(GLOB BOOST_MODULE_LIST RELATIVE
    ${CMAKE_CURRENT_LIST_DIR}/libs
    ${CMAKE_CURRENT_LIST_DIR}/libs/*)

  list(REMOVE_ITEM BOOST_MODULE_LIST "test")

  foreach(BOOST_MODULE ${BOOST_MODULE_LIST})
    if (IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/libs/${BOOST_MODULE})
      file(GLOB_RECURSE LOCALE_SOURCES RELATIVE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/libs/${BOOST_MODULE}/src/*.cpp)
      foreach(BOOST_SOURCE ${LOCALE_SOURCES})
        message("\${CMAKE_CURRENT_LIST_DIR}/${BOOST_SOURCE}")
      endforeach()
    endif()
  endforeach()
endfunction()

# PrintSourceFiles()

set(BOOST_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/libs/system/src/error_code.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/filesystem/src/codecvt_error_category.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/filesystem/src/codecvt_error_category.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/filesystem/src/operations.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/filesystem/src/path.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/filesystem/src/path_traits.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/filesystem/src/portability.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/filesystem/src/unique_path.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/filesystem/src/utf8_codecvt_facet.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/filesystem/src/windows_file_codecvt.cpp
  )

add_library(filesystem STATIC ${BOOST_SOURCES})

target_include_directories(filesystem
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})

target_compile_features(filesystem
  PUBLIC
    cxx_auto_type
    cxx_decltype
    cxx_final
    cxx_lambdas
    cxx_nullptr
    cxx_override)

target_compile_definitions(filesystem
  PUBLIC
    -DBOOST_ALL_NO_LIB)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  add_executable(test-linkage link-test/main.cpp)
  target_link_libraries(test-linkage filesystem)
endif()
