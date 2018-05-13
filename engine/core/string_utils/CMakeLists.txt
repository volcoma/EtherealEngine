file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (string_utils ${libsrc})

set_target_properties(string_utils PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(string_utils ultra)
