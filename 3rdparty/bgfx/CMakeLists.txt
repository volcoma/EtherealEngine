if(MSVC)
    add_compile_options(/wd4244 /wd4267 /wd4291)
endif()

add_subdirectory(bgfx)
