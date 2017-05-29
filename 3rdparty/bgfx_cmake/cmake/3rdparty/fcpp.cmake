# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET fcpp )
	return()
endif()

file( GLOB FCPP_SOURCES ${BGFX_DIR}/3rdparty/fcpp/*.c ${BGFX_DIR}/3rdparty/fcpp/*.h )

add_library( fcpp STATIC ${FCPP_SOURCES} )
target_include_directories( fcpp PUBLIC ${BGFX_DIR}/3rdparty/fcpp )
if( MSVC )
	set_target_properties( fcpp PROPERTIES COMPILE_FLAGS "/W0" )
endif()
set_target_properties( fcpp PROPERTIES FOLDER "bgfx/3rdparty" )
set_source_files_properties( ${BGFX_DIR}/3rdparty/fcpp/usecpp.c PROPERTIES HEADER_FILE_ONLY ON )
