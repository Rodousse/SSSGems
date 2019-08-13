
cmake_minimum_required(VERSION 2.8)

project(SSSGems)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

option (USE32bit 0)

set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )

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
        find_library(GLEW_LIBRARY glew32s HINTS ${GLEW_LIBS_DIR})
        set(GLEW_INCLUDE_DIR ${GLEW_PATH}/include)
    else()
        message(FATAL_ERROR "Please install glew before using this project, You can find the sources in third_party/glew")
    endif()
endif()

set(SOURCES
    src/main.cpp
    shaders/vertex.vert
    shaders/fragment.frag
    )
add_executable(SSSGems ${SOURCES})
include_directories( ${OPENGL_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})
target_link_libraries(SSSGems glfw ${OPENGL_LIBRARIES}  ${GLEW_LIBRARY} )
