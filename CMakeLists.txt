cmake_minimum_required(VERSION 3.5)

project (ethereal)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(ECM_ENABLE_SANITIZERS "custom")

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# if(NOT LINUX) should work, too, if you need that
if(LINUX)
    message(STATUS "OS >>> Linux")
    # linux stuff here
elseif(WIN32)
    message(STATUS "OS >>> Windows")
    # stuff that should happen not on Linux
endif()


message(STATUS "COMPILER >>> ${CMAKE_CXX_COMPILER_ID}")

option(BUILD_SHARED_LIBS
  "Build package with shared libraries."
  OFF
)

message(STATUS "BUILD_SHARED_LIBS >>> ${BUILD_SHARED_LIBS}")

add_definitions(-DENGINE_DIRECTORY="${PROJECT_SOURCE_DIR}")

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
endif()

function(add_subdirectory_ex arg1)
	message(STATUS "---------Configure ${arg1}")
	add_subdirectory(${arg1})
	message(STATUS "---------Done ${arg1}")
endfunction(add_subdirectory_ex)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /bigobj")
    set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} /IGNORE:4221")
elseif(MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3" )
endif()

message(STATUS "PROJECT_BINARY_DIR >>> ${PROJECT_BINARY_DIR}")
message(STATUS "CMAKE_CXX_FLAGS >>> ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG >>> ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE >>> ${CMAKE_CXX_FLAGS_RELEASE}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin) 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

include(ECMEnableSanitizers)

add_subdirectory_ex(engine)
add_subdirectory_ex(editor)
add_subdirectory_ex(3rdparty)
