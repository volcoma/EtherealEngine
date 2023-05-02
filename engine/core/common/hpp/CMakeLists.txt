set(target_name egt_hpp)

add_library(${target_name} INTERFACE)
target_sources(${target_name} INTERFACE)

target_include_directories(${target_name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set (target_for_ide "${target_name}_ide")
if (NOT TARGET ${target_for_ide})
    file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
    set(TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    file(GLOB_RECURSE TESTS_SOURCES "${TESTS_DIR}/*.c"
                                    "${TESTS_DIR}/*.cpp"
                                    "${TESTS_DIR}/*.h"
                                    "${TESTS_DIR}/*.hpp")
    list(REMOVE_ITEM SOURCES ${TESTS_SOURCES})
    add_custom_target(${target_for_ide} SOURCES ${SOURCES})
endif()

###############################################################################################

set(tests_target_name "${target_name}_tests")
add_library(${tests_target_name} EXCLUDE_FROM_ALL ${TESTS_SOURCES})

# Add definitions
set_target_properties(${tests_target_name} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_link_libraries(${tests_target_name} PUBLIC egt_suite)
target_link_libraries(${tests_target_name} PUBLIC ${target_name})
target_include_directories(${tests_target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
