cmake_minimum_required(VERSION 3.16)
project(MiniGolf LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 QUIET)
find_package(glm QUIET)

if(NOT glfw3_FOUND OR NOT glm_FOUND)
  include(FetchContent)
endif()
if(NOT glfw3_FOUND)
  message(STATUS "GLFW not found on the system - fetching it")
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
  FetchContent_MakeAvailable(glfw)
endif()
if(NOT glm_FOUND)
  message(STATUS "GLM not found on the system - fetching it")
  FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
  FetchContent_MakeAvailable(glm)
endif()

add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad PUBLIC external/glad/include)

add_executable(minigolf
  src/main.cpp
  src/Shader.cpp
)
target_include_directories(minigolf PRIVATE include src)
target_link_libraries(minigolf PRIVATE glad glfw glm::glm OpenGL::GL)
if(MSVC)
  target_compile_options(minigolf PRIVATE /W3)
else()
  target_compile_options(minigolf PRIVATE -Wall -Wextra)
endif()

add_custom_command(TARGET minigolf POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:minigolf>/shaders)
