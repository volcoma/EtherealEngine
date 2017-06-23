add_subdirectory_ex(filedialog)
target_include_directories (filedialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory_ex(assimp)
target_include_directories (assimp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/assimp/include)

add_subdirectory_ex(bgfx)
target_include_directories (bx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/bx/include)
target_include_directories (bimg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/bimg/include)
target_include_directories (bgfx PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/bgfx/bgfx/src}
${CMAKE_CURRENT_SOURCE_DIR}/bgfx/bgfx/include)

add_subdirectory_ex(tracey)
target_include_directories (tracey PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory_ex(rttr)

add_subdirectory_ex(cereal)

add_subdirectory_ex(mml)
target_include_directories (mml-window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mml/include)

add_subdirectory_ex(boost_filesystem)
target_include_directories (boost_filesystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory_ex(glm)
target_include_directories (glm_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glm)

add_subdirectory_ex(spdlog)
