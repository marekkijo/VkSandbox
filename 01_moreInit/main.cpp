/*#include <iostream>
#include <vulkan/vulkan.h>

int main(int argc, char *argv[]) {
    VkInstanceCreateInfo create_info{};
    VkAllocationCallbacks alloc_callbacks{};
    VkInstance instance{};

    VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
    return 0;
}
*/
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <vector>

void list_instance_extension_properties();
void list_device_extension_properties();

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    list_instance_extension_properties();
    list_device_extension_properties();

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

void list_instance_extension_properties() {
    std::uint32_t propertyCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);
    std::cout << propertyCount << " instance extension properties count" << std::endl;

    if (propertyCount == 0)
        return;

    std::vector<VkExtensionProperties> properties(propertyCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data());

    for (int i = 0; i < propertyCount; i++) {
        std::cout << i << ")\textensionName: " << properties[i].extensionName << " specVersion: " << properties[i].specVersion << std::endl;
    }
}

void list_device_extension_properties() {

}
