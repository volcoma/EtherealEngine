# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET ocornut-imgui )
	return()
endif()

file( GLOB OCORNUT_IMGUI_SOURCES ${BGFX_DIR}/3rdparty/ocornut-imgui/*.cpp ${BGFX_DIR}/3rdparty/ocornut-imgui/*.h ${BGFX_DIR}/3rdparty/ocornut-imgui/*.inl )

add_library( ocornut-imgui STATIC EXCLUDE_FROM_ALL ${OCORNUT_IMGUI_SOURCES} )
target_compile_definitions( ocornut-imgui PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" )
target_include_directories( ocornut-imgui PUBLIC ${BGFX_DIR}/3rdparty )
target_link_libraries( ocornut-imgui PUBLIC bx )
set_target_properties( ocornut-imgui PROPERTIES FOLDER "bgfx/3rdparty" )
