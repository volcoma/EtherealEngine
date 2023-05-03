if(MSVC)
    add_compile_options(/wd4244 /wd4267 /wd4291)
endif()

if(MINGW)
    add_compile_definitions(ASTCENC_SSE=0)
endif()
add_subdirectory(bgfx)
