# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET etc2 )
	return()
endif()

file( GLOB ETC2_SOURCES ${BIMG_DIR}/3rdparty/etc2/*.cpp ${BIMG_DIR}/3rdparty/etc2/*.h )

add_library( etc2 STATIC ${ETC2_SOURCES} )
target_include_directories( etc2 PUBLIC ${BIMG_DIR}/3rdparty )
set_target_properties( etc2 PROPERTIES FOLDER "bgfx/3rdparty" )
target_link_libraries( etc2 PUBLIC bx )
