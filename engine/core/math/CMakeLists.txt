file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (math ${libsrc})

target_link_libraries(math PUBLIC glm_static)

set_target_properties(math PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(math high)
