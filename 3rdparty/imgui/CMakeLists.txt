set(libsrc 
imgui/imgui.cpp
imgui/imgui.h
imgui/imgui_draw.cpp
imgui/imgui_demo.cpp
imgui_user/imgui_user.cpp
imgui_user/imgui_user.h
imguizmo/imguizmo.cpp
imguizmo/imguizmo.h)

add_library (imgui ${libsrc})

if(MSVC)
set(imgui_flags
_CRT_SECURE_NO_WARNINGS
_CRT_SECURE_NO_DEPRECATE)
endif()

target_compile_definitions(imgui PUBLIC ${imgui_flags})
target_include_directories (imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
