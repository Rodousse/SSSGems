set(SOURCES
    data/Mesh.h)

set(SOURCES_PRIVATE
    data/data.cpp)

#DO NOT LINK THAT LIBRARY
#Only for convenient file editing properties in IDE
add_library(data_private SHARED EXCLUDE_FROM_ALL)
target_sources(data_private PRIVATE  ${SOURCES} ${SOURCES_PRIVATE})
target_link_libraries(data_private PRIVATE glm::glm)

add_library(data INTERFACE)
target_include_directories(data INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(data INTERFACE glm::glm)
