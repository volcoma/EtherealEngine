if(MSVC)
    add_compile_options(/wd4244)
    add_compile_options(/wd4273)
    add_compile_options(/wd4018)
    add_compile_options(/wd4334)
    add_compile_options(/wd4267)
    add_compile_options(/wd4065)
endif()


add_subdirectory(bgfx)
