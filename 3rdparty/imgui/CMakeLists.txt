file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc *.m)

add_library (imgui ${libsrc})

if(MSVC)
set(imgui_flags
_CRT_SECURE_NO_WARNINGS
_CRT_SECURE_NO_DEPRECATE)
endif()

target_compile_definitions(imgui PUBLIC ${imgui_flags})
