cmake_minimum_required(VERSION 3.7)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/khronos)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/dxsdk/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/bx/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/nvtt)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/bgfx)

add_subdirectory_ex(filedialog)
target_include_directories (filedialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory_ex(assimp)
target_include_directories (assimp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/assimp/include)

add_subdirectory_ex(edtaa3)

add_subdirectory_ex(etc1)

add_subdirectory_ex(etc2)

add_subdirectory_ex(iqa)
target_include_directories (iqa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/iqa/include)

add_subdirectory_ex(libsquish)

add_subdirectory_ex(lodepng)

add_subdirectory_ex(nvtt)

add_subdirectory_ex(pvrtc)

add_subdirectory_ex(bx)
target_include_directories (bx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bx/include)

add_subdirectory_ex(bimg)
target_include_directories (bimg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bimg/include)

add_subdirectory_ex(bgfx)
target_include_directories (bgfx PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/bgfx/include)

add_subdirectory_ex(glslang)
target_include_directories (glslang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory_ex(glsl-optimizer)
target_include_directories (glsl_optimizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory_ex(fcpp)
target_include_directories (fcpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory_ex(tracey)
target_include_directories (tracey PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if(MSVC)
set(glsl_optimizer_flags
__STDC_LIMIT_MACROS
__STDC_FORMAT_MACROS
__STDC_CONSTANT_MACROS
__STDC__
__STDC_VERSION__=199901L
_CRT_SECURE_NO_WARNINGS
_CRT_SECURE_NO_DEPRECATE
strdup=_strdup
alloca=_alloca
isascii=__isascii
NINCLUDE=64
NWORK=65536
NBUFF=65536
OLD_PREPROCESSOR=0)
endif()

target_compile_definitions(glcpp-library PUBLIC ${glsl_optimizer_flags})
target_compile_definitions(mesa PUBLIC ${glsl_optimizer_flags})