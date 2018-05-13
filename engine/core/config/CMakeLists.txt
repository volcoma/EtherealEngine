file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (config ${libsrc})

set_target_properties(config PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(config ultra)
