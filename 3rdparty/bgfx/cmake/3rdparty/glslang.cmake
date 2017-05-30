# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET glslang )
	return()
endif()

file( GLOB GLSLANG_SOURCES
	${BGFX_DIR}/3rdparty/glslang/glslang/GenericCodeGen/*.cpp
	${BGFX_DIR}/3rdparty/glslang/glslang/MachineIndependent/*.cpp
	${BGFX_DIR}/3rdparty/glslang/glslang/MachineIndependent/preprocessor/*.cpp
	${BGFX_DIR}/3rdparty/glslang/hlsl/*.cpp
	${BGFX_DIR}/3rdparty/glslang/SPIRV/*.cpp
	${BGFX_DIR}/3rdparty/glslang/OGLCompilersDLL/*.cpp
)

if( WIN32 )
	list( APPEND GLSLANG_SOURCES ${BGFX_DIR}/3rdparty/glslang/glslang/OSDependent/Windows/ossource.cpp )
else()
	list( APPEND GLSLANG_SOURCES ${BGFX_DIR}/3rdparty/glslang/glslang/OSDependent/Unix/ossource.cpp )
endif()

add_library( glslang STATIC EXCLUDE_FROM_ALL ${GLSLANG_SOURCES} )
target_include_directories( glslang PUBLIC ${BGFX_DIR}/3rdparty/glslang ${BGFX_DIR}/3rdparty/glslang/glslang/Include ${BGFX_DIR}/3rdparty/glslang/glslang/Public )
set_target_properties( glslang PROPERTIES FOLDER "bgfx/3rdparty" )
