file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (graphics ${libsrc})

target_link_libraries(graphics PUBLIC bgfx bx)
target_compile_definitions( graphics PRIVATE "MAX_RENDER_PASSES=${MAX_VIEWS}" )

set_target_properties(graphics PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(graphics high)
