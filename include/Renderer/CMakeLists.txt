add_library(renderer Renderer.cpp Renderer.h) 

target_include_directories(renderer PUBLIC ${PROJECT_BINARY_DIR}/extern/src)
target_include_directories(renderer PUBLIC ${PROJECT_SOURCE_DIR}/VulkanLayer/src)
target_include_directories(renderer PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(renderer glfw glm)
target_link_libraries(renderer vulkan_layer)

find_package(Vulkan REQUIRED)
find_package(OpenAL REQUIRED)

target_link_libraries(renderer ${Vulkan_LIBRARIES})
target_include_directories(renderer PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(renderer ${OPENAL_LIBRARY})
target_include_directories(renderer PUBLIC ${OPENAL_INCLUDE_DIR})

target_compile_features(renderer PUBLIC cxx_std_17)

source_group(
	TREE "${PROJECT_SOURCE_DIR}/include/Renderer"
  PREFIX "Header Files"
  FILES Renderer.h)
