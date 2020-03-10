cmake_minimum_required(VERSION 3.11)


project(SSSGems
    VERSION 1.0.0
    DESCRIPTION "Implementation of one of the GPU Gems subsurface scattering implementation."
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################### THIRD PARTY ###########################################

find_package (OpenGL REQUIRED)

############# Submodules #############
add_definitions(-DGLEW_STATIC)
set(glew-cmake_BUILD_SHARED OFF)
set(glew-cmake_BUILD_MULTI_CONTEXT OFF)
add_subdirectory(third_party/glew-cmake)
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glm)
add_definitions(-DTINYOBJLOADER_IMPLEMENTATION)
add_subdirectory(third_party/tinyobjloader)


########################################### CMAKE CONFIGURE FILE ##############################


set(RESOURCE_PATH ${PROJECT_SOURCE_DIR}/resources)
configure_file(cmake/defines.h.in include/defines.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}/include/)


########################################### Sources ###########################################

add_subdirectory(src/data)
add_subdirectory(src/utils)
add_subdirectory(src/app)


