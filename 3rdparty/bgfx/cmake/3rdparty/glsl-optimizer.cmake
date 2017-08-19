# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET glsl-optimizer )
	return()
endif()

set( GLSL-OPTIMIZER_INCLUDES
	${BGFX_DIR}/3rdparty/glsl-optimizer/include
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/mesa
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/mapi
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl
	${BGFX_DIR}/3rdparty/glsl-optimizer/src
)


if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
	if(CMAKE_BUILD_TYPE MATCHES Debug)
	else()
		string (REPLACE "-O2" "-O1" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
		string (REPLACE "-O3" "-O1" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
endif()

# glcpp
file( GLOB GLCPP_SOURCES ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/glcpp/*.c ${BGFX_DIR}/3rdparty/glsl-optimizer/src/util/*.c )
add_library( glcpp ${GLCPP_SOURCES} )
target_include_directories( glcpp PUBLIC ${GLSL-OPTIMIZER_INCLUDES} )
if(MSVC)
	target_compile_definitions( glcpp PRIVATE
			"__STDC__"
			"__STDC_VERSION__=199901L"
			"strdup=_strdup"
			"alloca=_alloca"
			"isascii=__isascii"
			"atoll=_atoi64"
			"strtoll=_strtoi64"
			"strtoull=_strtoui64" 
		)
endif()

set_target_properties( glcpp PROPERTIES FOLDER "bgfx/3rdparty" )

# mesa
file( GLOB MESA_SOURCES ${BGFX_DIR}/3rdparty/glsl-optimizer/src/mesa/program/*.c ${BGFX_DIR}/3rdparty/glsl-optimizer/src/mesa/main/*.c )
add_library( mesa ${MESA_SOURCES} )
target_include_directories( mesa PUBLIC ${GLSL-OPTIMIZER_INCLUDES} )
if(MSVC)
	target_compile_definitions( mesa PRIVATE
			"__STDC__"
			"__STDC_VERSION__=199901L"
			"strdup=_strdup"
			"alloca=_alloca"
			"isascii=__isascii"
			"atoll=_atoi64"
			"strtoll=_strtoi64"
			"strtoull=_strtoui64" 
		)
endif()
set_target_properties( mesa PROPERTIES FOLDER "bgfx/3rdparty" )

# glsl-optimizer
file( GLOB GLSL-OPTIMIZER_SOURCES ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/*.cpp ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/*.c )
file( GLOB GLSL-OPTIMIZER_SOURCES_REMOVE ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/main.cpp ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/builtin_stubs.cpp )
list( REMOVE_ITEM GLSL-OPTIMIZER_SOURCES ${GLSL-OPTIMIZER_SOURCES_REMOVE} )
add_library( glsl-optimizer ${GLSL-OPTIMIZER_SOURCES} )
target_link_libraries( glsl-optimizer glcpp mesa )
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_link_libraries( glsl-optimizer m )
endif()
if(MSVC)
	target_compile_definitions(glsl-optimizer PRIVATE
			"__STDC__"
			"__STDC_VERSION__=199901L"
			"strdup=_strdup"
			"alloca=_alloca"
			"isascii=__isascii"
			"atoll=_atoi64"
			"strtoll=_strtoi64"
			"strtoull=_strtoui64" 
		)

	target_compile_options( glsl-optimizer PRIVATE
		"/wd4291" 
		"/wd4100" 
		"/wd4127" 
		"/wd4132"
		"/wd4189" 
		"/wd4204" 
		"/wd4244" 
		"/wd4389"
		"/wd4245" 
		"/wd4701" 
		"/wd4702" 
		"/wd4706" 
		"/wd4996" )
endif()
set_target_properties( glsl-optimizer PROPERTIES FOLDER "bgfx/3rdparty" )
