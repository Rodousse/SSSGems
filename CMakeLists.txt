
cmake_minimum_required(VERSION 2.8)

project(SSSGems)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(TinyObjLoader REQUIRED)

option (USE32bit 0)

set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
set(RESOURCE_PATH ${PROJECT_SOURCE_DIR}/resources)

configure_file(cmake/defines.h.in include/defines.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}/include/)

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/glfw)

find_package(GLEW)
if(NOT ${GLEW_FOUND})
    if(${WIN32})
        set(GLEW_PATH ${PROJECT_SOURCE_DIR}/third_party/glew/glewWin-2.1.0)
        if(USE32bit)
            set(GLEW_LIBS_DIR ${GLEW_PATH}/lib/Release/Win32)
        else()
            set(GLEW_LIBS_DIR ${GLEW_PATH}/lib/Release/x64)
        endif()
        find_library(GLEW_LIBRARIES glew32s HINTS ${GLEW_LIBS_DIR})
        set(GLEW_INCLUDE_DIRS ${GLEW_PATH}/include)
    else()
        message(FATAL_ERROR "Please install glew before using this project, You can find the sources in third_party/glew")
    endif()
endif()

set(SOURCES
    src/main.cpp
    src/utils/ShaderUtils.cpp
    src/utils/ObjLoader.cpp
    resources/shaders/vertex.vert
    resources/shaders/fragment.frag
    )

set(HEADERS
    include/Mesh.h
    include/utils/ShaderUtils.h
    include/utils/ObjLoader.h)

add_executable(SSSGems ${SOURCES} ${HEADERS})

target_include_directories(SSSGems PRIVATE
    ${PROJECT_SOURCE_DIR}/include/
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIR}
    ${TINYOBJLOADER_INCLUDE_DIR})

target_link_libraries(SSSGems
    glfw
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES})
