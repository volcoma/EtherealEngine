# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include( CMakeParseArguments )

include( cmake/3rdparty/forsyth-too.cmake )
include( cmake/3rdparty/ib-compress.cmake )

add_executable( geometryc ${BGFX_DIR}/tools/geometryc/geometryc.cpp )
target_compile_definitions( geometryc PRIVATE "-D_CRT_SECURE_NO_WARNINGS" )
set_target_properties( geometryc PROPERTIES FOLDER "bgfx/tools" )
target_link_libraries( geometryc bx bgfx-bounds bgfx-vertexdecl forsyth-too ib-compress )
if( BGFX_CUSTOM_TARGETS )
	add_dependencies( tools geometryc )
endif()
