file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (serialization ${libsrc})

target_link_libraries(serialization PUBLIC cereal)

set_target_properties(serialization PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(serialization ultra)
