add_executable(app DodgingBoxes.cpp)
target_compile_features(app PRIVATE cxx_std_17)

target_include_directories(app PUBLIC ../src)
target_include_directories(app PUBLIC ../VulkanLayer/src)
target_include_directories(app PUBLIC ../include)

target_link_libraries(app dodgin_boxes vulkan_layer glfw glm)

target_link_libraries(app ${Vulkan_LIBRARIES})
target_include_directories(app PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(app ${OPENAL_LIBRARIES})
target_include_directories(app PUBLIC ${OPENAL_INCLUDE_DIR})
