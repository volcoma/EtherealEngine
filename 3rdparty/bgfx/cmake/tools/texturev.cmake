# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include( CMakeParseArguments )

include( cmake/3rdparty/ib-compress.cmake )
include( cmake/3rdparty/ocornut-imgui.cmake )
include( cmake/3rdparty/remotery.cmake )

function( add_example ARG_NAME )
	# Parse arguments
	cmake_parse_arguments( ARG "COMMON" "" "DIRECTORIES;SOURCES" ${ARGN} )

	# Get all source files
	list( APPEND ARG_DIRECTORIES "${BGFX_DIR}/examples/${ARG_NAME}" )
	set( SOURCES "" )
	set( SHADERS "" )
	foreach( DIR ${ARG_DIRECTORIES} )
		if( APPLE )
			file( GLOB GLOB_SOURCES ${DIR}/*.mm )
			list( APPEND SOURCES ${GLOB_SOURCES} )
		endif()
		file( GLOB GLOB_SOURCES ${DIR}/*.c ${DIR}/*.cpp ${DIR}/*.h ${DIR}/*.sc )
		list( APPEND SOURCES ${GLOB_SOURCES} )
		file( GLOB GLOB_SHADERS ${DIR}/*.sc )
		list( APPEND SHADERS ${GLOB_SHADERS} )
	endforeach()

	# Add target
	if( ARG_COMMON )
		add_library( example-${ARG_NAME} STATIC EXCLUDE_FROM_ALL ${SOURCES} )
		target_include_directories( example-${ARG_NAME} PUBLIC ${BGFX_DIR}/examples/common )
		target_link_libraries( example-${ARG_NAME} PUBLIC bgfx ib-compress ocornut-imgui remotery )
		if( UNIX AND NOT APPLE )
			target_link_libraries( example-${ARG_NAME} PUBLIC X11 )
		endif()

	else()
		if( BGFX_INSTALL_EXAMPLES )
			add_executable( example-${ARG_NAME} WIN32 ${SOURCES} )
		else()
			add_executable( example-${ARG_NAME} WIN32 EXCLUDE_FROM_ALL ${SOURCES} )
		endif()
		target_link_libraries( example-${ARG_NAME} example-common )
		configure_debugging( example-${ARG_NAME} WORKING_DIR ${BGFX_DIR}/examples/runtime )
		if( MSVC )
			set_target_properties( example-${ARG_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
		endif()
		if( BGFX_CUSTOM_TARGETS )
			add_dependencies( examples example-${ARG_NAME} )
		endif()
	endif()
	target_compile_definitions( example-${ARG_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" "-DENTRY_CONFIG_IMPLEMENT_MAIN=1" )

	# Configure shaders
	if( NOT ARG_COMMON )
		foreach( SHADER ${SHADERS} )
			add_bgfx_shader( ${SHADER} ${ARG_NAME} )
		endforeach()
		source_group( "Shader Files" FILES ${SHADERS})
	endif()

	# Directory name
	set_target_properties( example-${ARG_NAME} PROPERTIES FOLDER "bgfx/examples" )
endfunction()


# Add common library for examples
add_example(
	common
	COMMON
	DIRECTORIES
	${BGFX_DIR}/examples/common/debugdraw
	${BGFX_DIR}/examples/common/entry
	${BGFX_DIR}/examples/common/font
	${BGFX_DIR}/examples/common/imgui
	${BGFX_DIR}/examples/common/nanovg
	${BGFX_DIR}/examples/common/ps
)


add_executable( texturev ${BGFX_DIR}/tools/texturev/texturev.cpp )
set_target_properties( texturev PROPERTIES FOLDER "bgfx/tools" )
target_link_libraries( texturev PUBLIC example-common )

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_link_libraries(texturev PUBLIC "-stdlib=libstdc++ -lstdc++")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_link_libraries(texturev PUBLIC "-static")
endif()

if( BGFX_CUSTOM_TARGETS )
	add_dependencies( tools texturev )
endif()
