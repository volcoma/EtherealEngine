file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (system ${libsrc})

target_link_libraries(system PUBLIC common_lib)

set_target_properties(system PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(system ultra)
