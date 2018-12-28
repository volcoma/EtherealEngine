# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

cmake_minimum_required( VERSION 3.0 )
project( bgfx )

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
if( APPLE )
	set( CMAKE_CXX_FLAGS "-ObjC++ --std=c++14" )
elseif(UNIX)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

option( BGFX_BUILD_TOOLS    "Build bgfx tools."                             ON  )
option( BGFX_BUILD_EXAMPLES "Build bgfx examples."                          OFF  )
option( BGFX_INSTALL        "Create installation target."                   ON  )
option( BGFX_CUSTOM_TARGETS "Include convenience custom targets."           ON  )
option( BGFX_USE_OVR        "Build with OVR support."                       OFF )
option( BGFX_AMALGAMATED    "Amalgamated bgfx build for faster compilation" OFF )
option( BX_AMALGAMATED      "Amalgamated bx build for faster compilation"   OFF )

if( NOT BX_DIR )
	set( BX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bx" CACHE STRING "Location of bx." )
endif()
if( NOT BIMG_DIR )
	set( BIMG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bimg" CACHE STRING "Location of bimg." )
endif()
if( NOT BGFX_DIR )
	set( BGFX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bgfx" CACHE STRING "Location of bgfx." )
endif()

if( BGFX_USE_OVR )
	include( cmake/ovr.cmake )
endif()

include( cmake/shared.cmake )
include( cmake/bx.cmake )
include( cmake/bimg.cmake )
include( cmake/bgfx.cmake )

if( BGFX_BUILD_TOOLS )
	include( cmake/tools.cmake )
endif()

if( BGFX_BUILD_EXAMPLES )
	include( cmake/examples.cmake )
endif()

if( BGFX_INSTALL )
	# install bx
	install( TARGETS bx DESTINATION lib )
	install( DIRECTORY ${BX_DIR}/include DESTINATION . )

	# install bimg
	install( TARGETS bimg DESTINATION lib )
	install( DIRECTORY ${BIMG_DIR}/include DESTINATION . )

	# install bgfx
	install( TARGETS bgfx DESTINATION lib )
	install( DIRECTORY ${BGFX_DIR}/include DESTINATION . )

	# install tools
	if( BGFX_BUILD_TOOLS )
        install( TARGETS shaderc DESTINATION lib )
		install( TARGETS texturec DESTINATION lib )
	endif()
endif()
