set(INCROOT ${PROJECT_SOURCE_DIR}/include/mml/system)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/mml/system)

# all source files
set(SRC
    ${SRCROOT}/err.cpp
    ${INCROOT}/err.hpp
    ${INCROOT}/export.hpp
    ${INCROOT}/non_copyable.hpp
    ${INCROOT}/utf.hpp
    ${INCROOT}/utf.inl
)
source_group("" FILES ${SRC})

# define the mml-system target
add_library(mml-system ${SRC} ${PLATFORM_SRC})
if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(mml-system PUBLIC MML_STATIC)
endif()