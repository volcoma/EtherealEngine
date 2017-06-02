cmake_minimum_required(VERSION 2.8.3)

set(INCROOT ${PROJECT_SOURCE_DIR}/include/mml/window)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/mml/window)

# all source files
set(SRC
    ${SRCROOT}/cursor.cpp
    ${INCROOT}/cursor.hpp
    ${SRCROOT}/cursor_impl.hpp
    ${INCROOT}/export.hpp
    ${INCROOT}/event.hpp
    ${SRCROOT}/input_impl.hpp
    ${INCROOT}/joystick.hpp
    ${SRCROOT}/joystick.cpp
    ${SRCROOT}/joystick_impl.hpp
    ${SRCROOT}/joystick_manager.cpp
    ${SRCROOT}/joystick_manager.hpp
    ${INCROOT}/keyboard.hpp
    ${SRCROOT}/keyboard.cpp
    ${INCROOT}/mouse.hpp
    ${SRCROOT}/mouse.cpp
    ${INCROOT}/touch.hpp
    ${SRCROOT}/touch.cpp
    ${INCROOT}/sensor.hpp
    ${SRCROOT}/sensor.cpp
    ${SRCROOT}/sensor_impl.hpp
    ${SRCROOT}/sensor_manager.cpp
    ${SRCROOT}/sensor_manager.hpp
    ${SRCROOT}/video_mode.cpp
    ${INCROOT}/video_mode.hpp
    ${SRCROOT}/video_mode_impl.hpp
    ${SRCROOT}/window.cpp
    ${INCROOT}/window.hpp
    ${INCROOT}/window_handle.hpp
    ${SRCROOT}/window_impl.cpp
    ${SRCROOT}/window_impl.hpp
    ${INCROOT}/window_style.hpp
)

source_group("" FILES ${SRC})

# add platform specific sources
if(MML_OS_WINDOWS)
    set(PLATFORM_SRC
        ${SRCROOT}/win32/cursor_impl.hpp
        ${SRCROOT}/win32/cursor_impl.cpp
        ${SRCROOT}/win32/input_impl.cpp
        ${SRCROOT}/win32/input_impl.hpp
        ${SRCROOT}/win32/joystick_impl.cpp
        ${SRCROOT}/win32/joystick_impl.hpp
        ${SRCROOT}/win32/sensor_impl.hpp
        ${SRCROOT}/win32/sensor_impl.cpp
        ${SRCROOT}/win32/video_mode_impl.cpp
        ${SRCROOT}/win32/window_impl_win32.cpp
        ${SRCROOT}/win32/window_impl_win32.hpp
    )
    source_group("windows" FILES ${PLATFORM_SRC})

    # make sure that we use the Unicode version of the Win API functions
    add_definitions(-DUNICODE -D_UNICODE)
elseif(MML_OS_LINUX OR MML_OS_FREEBSD)
    set(PLATFORM_SRC
        ${SRCROOT}/unix/cursor_impl.hpp
        ${SRCROOT}/unix/cursor_impl.cpp
        ${SRCROOT}/unix/display.cpp
        ${SRCROOT}/unix/display.hpp
        ${SRCROOT}/unix/input_impl.cpp
        ${SRCROOT}/unix/input_impl.hpp
        ${SRCROOT}/unix/sensor_impl.cpp
        ${SRCROOT}/unix/sensor_impl.hpp
        ${SRCROOT}/unix/video_mode_impl.cpp
        ${SRCROOT}/unix/window_impl_x11.cpp
        ${SRCROOT}/unix/window_impl_x11.hpp
    )
    if(MML_OS_LINUX)
        set(PLATFORM_SRC
            ${PLATFORM_SRC}
            ${SRCROOT}/unix/joystick_impl.cpp
            ${SRCROOT}/unix/joystick_impl.hpp
        )
    elseif(MML_OS_FREEBSD)
        set(PLATFORM_SRC
            ${PLATFORM_SRC}
            ${SRCROOT}/free_bsd/joystick_impl.cpp
            ${SRCROOT}/free_bsd/joystick_impl.hpp
        )
    endif()
    source_group("unix" FILES ${PLATFORM_SRC})
elseif(MML_OS_MACOSX)
elseif(MML_OS_IOS)
elseif(MML_OS_ANDROID)

endif()

# find external libraries
if(MML_OS_LINUX OR MML_OS_FREEBSD)
    find_package(X11 REQUIRED)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library not found")
    endif()
    if(NOT X11_Xrandr_FOUND)
        message(FATAL_ERROR "Xrandr library not found")
    endif()
    include_directories(${X11_INCLUDE_DIR})
endif()

if(MML_OS_LINUX)
    find_package(UDev REQUIRED)
    if(NOT UDEV_FOUND)
        message(FATAL_ERROR "udev library not found")
    endif()
    include_directories(${UDEV_INCLUDE_DIR})
endif()

# build the list of external libraries to link
if(MML_OS_WINDOWS)
    list(APPEND WINDOW_EXT_LIBS winmm gdi32)
elseif(MML_OS_LINUX)
    list(APPEND WINDOW_EXT_LIBS ${X11_X11_LIB} ${X11_Xrandr_LIB} ${UDEV_LIBRARIES})
elseif(MML_OS_FREEBSD)
    list(APPEND WINDOW_EXT_LIBS ${X11_X11_LIB} ${X11_Xrandr_LIB} usbhid)
endif()

# define the mml-window target
add_library(mml-window ${SRC} ${PLATFORM_SRC} )
target_link_libraries(mml-window PUBLIC mml-system ${WINDOW_EXT_LIBS})

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(mml-window PUBLIC MML_STATIC)
endif()
