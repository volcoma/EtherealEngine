# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include( CMakeParseArguments )


add_executable( texturec ${BIMG_DIR}/tools/texturec/texturec.cpp )
target_compile_definitions( texturec PRIVATE "-D_CRT_SECURE_NO_WARNINGS" )
set_target_properties( texturec PROPERTIES FOLDER "bgfx/tools" )
target_link_libraries( texturec PUBLIC bx bimg)
target_include_directories( texturec PUBLIC ${BIMG_DIR}/tools )
if( BGFX_CUSTOM_TARGETS )
	add_dependencies( tools texturec )
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_link_libraries(texturec PUBLIC "-stdlib=libstdc++ -lstdc++")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_link_libraries(texturec PUBLIC "-static")
endif()
