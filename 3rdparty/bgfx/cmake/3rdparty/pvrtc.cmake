# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET pvrtc )
	return()
endif()

file( GLOB PVRTC_SOURCES ${BIMG_DIR}/3rdparty/pvrtc/*.cpp ${BIMG_DIR}/3rdparty/pvrtc/*.h )

add_library( pvrtc STATIC ${PVRTC_SOURCES} )
target_include_directories( pvrtc PUBLIC ${BIMG_DIR}/3rdparty )
set_target_properties( pvrtc PROPERTIES FOLDER "bgfx/3rdparty" )
