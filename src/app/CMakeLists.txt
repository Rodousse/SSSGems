set(SOURCES
    main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    utils
    glfw
    libglew_static
    )
