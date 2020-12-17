set(HEADER_LIST Character.h Collider.h DeathScreen.h Enemy.h EnemyManager.h Font.h GameManager.h GameObject.h PauseScreen.h Player.h SoundManager.h Text.h Utilities.h)

add_library(dodgin_boxes Character.cpp Collider.cpp DeathScreen.cpp Enemy.cpp EnemyManager.cpp Font.cpp GameManager.cpp PauseScreen.cpp Player.cpp SoundManager.cpp Text.cpp Utilities.cpp ${HEADER_LIST})

target_include_directories(dodgin_boxes PUBLIC ${PROJECT_BINARY_DIR}/VulkanLayer/extern/src)
target_include_directories(dodgin_boxes PUBLIC ${PROJECT_BINARY_DIR}/extern/src)
target_include_directories(dodgin_boxes PUBLIC ${PROJECT_SOURCE_DIR}/VulkanLayer/src)
target_include_directories(dodgin_boxes PUBLIC ${PROJECT_SOURCE_DIR}/include/)

target_link_libraries(dodgin_boxes glfw glm)
target_link_libraries(dodgin_boxes vulkan_layer)
target_link_libraries(dodgin_boxes renderer)

target_link_libraries(dodgin_boxes ${Vulkan_LIBRARIES})
target_include_directories(dodgin_boxes PUBLIC ${Vulkan_INCLUDE_DIR})

target_include_directories(dodgin_boxes PUBLIC ${OpenAL_INCLUDE_DIR})
target_link_libraries(dodgin_boxes ${OpenAL_LIBRARIES})

target_compile_features(dodgin_boxes PUBLIC cxx_std_17)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/src"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

add_custom_command(TARGET dodgin_boxes PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/Resources ${PROJECT_BINARY_DIR}/app/Resources)
