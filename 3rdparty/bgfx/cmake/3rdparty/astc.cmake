# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET astc )
	return()
endif()

file( GLOB ASTC_SOURCES ${BIMG_DIR}/3rdparty/astc/*.cpp ${BIMG_DIR}/3rdparty/astc/*.h )

add_library( astc ${ASTC_SOURCES} )
target_include_directories( astc PUBLIC ${BIMG_DIR}/3rdparty )
set_target_properties( astc PROPERTIES FOLDER "bgfx/3rdparty" )
if( MSVC )
	target_compile_options( astc PRIVATE
		"/wd4005"
		"/wd4100"
		"/wd4127"
		"/wd4244"
		"/wd4456"
		"/wd4457"
		"/wd4458"
		"/wd4702"
	)
else()
	target_compile_options( astc PRIVATE
		"-Wno-deprecated-register"
		"-Wno-ignored-qualifiers"
		"-Wno-inconsistent-missing-override"
		"-Wno-missing-field-initializers"
		"-Wno-reorder"
		"-Wno-return-type"
		"-Wno-shadow"
		"-Wno-sign-compare"
		"-Wno-undef"
		"-Wno-unknown-pragmas"
		"-Wno-unused-parameter"
		"-Wno-unused-variable"
	)
endif()
