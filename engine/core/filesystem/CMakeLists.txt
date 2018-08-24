file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (filesystem ${libsrc})

target_link_libraries(filesystem PUBLIC boost_filesystem)

#if (CMAKE_COMPILER_IS_GNUCXX)
#	target_link_libraries(filesystem PUBLIC stdc++fs)
#elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#	target_link_libraries(filesystem PUBLIC c++experimental)
#endif()

set_target_properties(filesystem PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(filesystem high)
