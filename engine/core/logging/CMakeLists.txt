file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (logging ${libsrc})

target_link_libraries(logging PUBLIC spdlog common_lib)

set_target_properties(logging PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(logging ultra)
