set(SOURCES
    utils/ObjLoader.cpp
    utils/ShaderUtils.cpp)

set(HEADERS
    utils/ObjLoader.h
    utils/ShaderUtils.h)

add_library(utils ${SOURCES} ${HEADERS})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(utils
    PUBLIC
    data
    PRIVATE
    tinyobjloader)
