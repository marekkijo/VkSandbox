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

#include <iostream>
#include <string>
#include <vector>

void list_device_extension_properties();

class HelloTriangleApplication {
public:
    HelloTriangleApplication()
        : m_application_name{"Hello triangle"}
        , m_width{800}
        , m_height{600}
        , m_window{nullptr}
        , m_vk_instance{nullptr} {
    }

    ~HelloTriangleApplication() {
        cleanup();
    }

    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    std::string m_application_name;
    int m_width;
    int m_height;
    GLFWwindow *m_window;
    VkInstance m_vk_instance;

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_window = glfwCreateWindow(m_width, m_height, m_application_name.c_str(), nullptr, nullptr);
        if (!m_window) {
            throw std::runtime_error("glfwCreateWindow(...) failed");
        }
    }

    void initVulkan() {
        m_vk_instance = createVkInstance();
        if (!m_vk_instance) {
            throw std::runtime_error("createVkInstance(...) failed");
        }

        listInstanceExtensionProperties();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(m_window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        if (m_vk_instance) {
            vkDestroyInstance(m_vk_instance, nullptr);
            m_vk_instance = nullptr;
        }
        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        glfwTerminate();
    }

    VkInstance createVkInstance() const {
        VkInstance instance{nullptr};

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = m_application_name.c_str();
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "No engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&createInfo.enabledExtensionCount);

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result != VK_SUCCESS) {
            return nullptr;
        }
        return instance;
    }

    static void listInstanceExtensionProperties() {
        std::uint32_t propertyCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);
        std::cout << propertyCount << " instance extension property count" << std::endl;
        if (propertyCount == 0)
            return;

        std::vector<VkExtensionProperties> properties(propertyCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data());

        for (int i = 0; i < propertyCount; i++) {
            std::cout << i << ") extensionName: " << properties[i].extensionName << " specVersion: " << properties[i].specVersion << std::endl;
        }
    }
};

int main(int argc, char *argv[]) {
    HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}/*

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

void list_device_extension_properties() {

}
*/