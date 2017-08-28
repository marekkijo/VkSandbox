project(01_moreInit)

set(SOURCE_FILES
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
    PRIVATE ${VK_INCLUDE_DIR}
    PRIVATE ${GLFW_INCLUDE_DIR}
    PRIVATE ${GLM_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    ${VK_LIB}
    ${GLFW_LIB}
)
