cmake_minimum_required(VERSION 3.11...3.19)

project(
  DodginBoxes
  VERSION 1.0
  DESCRIPTION "A game about Dodgin' Boxes"
  LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

add_subdirectory(VulkanLayer)

add_subdirectory(extern)

add_subdirectory(include/Renderer)

add_subdirectory(src)

add_subdirectory(app)
