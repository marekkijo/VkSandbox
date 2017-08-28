cmake_minimum_required(VERSION 2.6.3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

if(WIN32)
    set(VK_SDK_PATH $ENV{VK_SDK_PATH})
    if(NOT ${VK_SDK_PATH})
        set(VK_SDK_PATH "C:\\VulkanSDK\\1.0.46.0")
    endif()
    set(VK_INCLUDE_DIR "${VK_SDK_PATH}\\Include")

    set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}\\ext\\glfw-3.2.1\\include")
    set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}\\ext\\glm-0.9.8.4\\glm")

    if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
        set(VK_LIB "${VK_SDK_PATH}\\Lib32\\vulkan-1.lib")
        set(GLFW_LIB "${CMAKE_SOURCE_DIR}\\ext\\glfw-3.2.1\\lib-vc2015\\glfw3.lib")
    else()
        set(VK_LIB "${VK_SDK_PATH}\\Lib\\vulkan-1.lib")
        set(GLFW_LIB "${CMAKE_SOURCE_DIR}\\ext\\glfw-3.2.1\\x64_lib-vc2015\\glfw3.lib")
    endif()
endif()

add_subdirectory(00_init)
add_subdirectory(01_moreInit)