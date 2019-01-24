#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

PFN_vkCreateDebugReportCallbackEXT vk_CreateDebugReportCallbackEXT{nullptr};
//#undef vkCreateDebugReportCallbackEXT
//PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT{nullptr};

template<typename Proc>
VkResult extractVkProc(VkInstance instance, const std::string &procName, Proc **proc) {
  PFN_vkVoidFunction procAddr = vkGetInstanceProcAddr(instance, procName.c_str());
  if (!procAddr) {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  *proc = reinterpret_cast<Proc *>(procAddr);
  return VK_SUCCESS;
}

const std::vector<std::string> validationLayers = {
  "VK_LAYER_LUNARG_standard_validation"
};

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

class HelloTriangleApplication {
public:
  HelloTriangleApplication()
    : m_applicationName{"Hello triangle"}
    , m_width{800}
    , m_height{600}
    , m_window{nullptr}
    , m_vk_instance{nullptr}
    , m_vk_debugReportCallback{0x0} {
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
  std::string m_applicationName;
  int m_width;
  int m_height;
  GLFWwindow *m_window;
  VkInstance m_vk_instance;
  VkDebugReportCallbackEXT m_vk_debugReportCallback;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(m_width, m_height, m_applicationName.c_str(), nullptr, nullptr);
    if (!m_window) {
      throw std::runtime_error("glfwCreateWindow(...) failed");
    }
  }

  void initVulkan() {
    m_vk_instance = createVkInstance();
    if (!m_vk_instance) {
      throw std::runtime_error("createVkInstance(...) failed");
    }

    if (ENABLE_VALIDATION_LAYERS) {
      if (extractVkProc(m_vk_instance, "vkCreateDebugReportCallbackEXT", &vk_CreateDebugReportCallbackEXT) != VK_SUCCESS) {
        throw std::runtime_error("cannot extract vkCreateDebugReportCallbackEXT procedure");
      }
    }

    m_vk_debugReportCallback = createVkDebugReportCallback();

    listInstanceExtensionProperties();
    listInstanceLayerProperties();
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(m_window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    if (m_vk_debugReportCallback) {
      // destroy debug report callback !!!
      m_vk_debugReportCallback = 0x0;
    }
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
    VkInstance vk_instance{nullptr};

    if (ENABLE_VALIDATION_LAYERS && !checkLayersSupport(validationLayers)) {
      throw std::runtime_error("validation layers requested, but not available!");
    }

    std::vector<const char *> enabledLayerNames = getEnabledLayerNames();
    std::vector<const char *> enabledExtensionNames = getEnabledExtensionNames();

    VkApplicationInfo vk_applicationInfo{};
    vk_applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_applicationInfo.pApplicationName = m_applicationName.c_str();
    vk_applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_applicationInfo.pEngineName = "No engine";
    vk_applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_applicationInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo vk_createInfo{};
    vk_createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_createInfo.pNext = nullptr;
    vk_createInfo.pApplicationInfo = &vk_applicationInfo;
    vk_createInfo.enabledLayerCount = enabledLayerNames.size();
    vk_createInfo.ppEnabledLayerNames = enabledLayerNames.data();
    vk_createInfo.enabledExtensionCount = enabledExtensionNames.size();
    vk_createInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

    VkResult vk_result = vkCreateInstance(&vk_createInfo, nullptr, &vk_instance);
    if (vk_result != VK_SUCCESS) {
      return nullptr;
    }
    return vk_instance;
  }

  VkDebugReportCallbackEXT createVkDebugReportCallback() {
    if (!ENABLE_VALIDATION_LAYERS) {
      return 0x0;
    }

    VkDebugReportCallbackEXT vk_debugReportCallback{0x0};

    VkDebugReportCallbackCreateInfoEXT vk_createInfo{};
    vk_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    vk_createInfo.flags =
      VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
      VK_DEBUG_REPORT_WARNING_BIT_EXT |
      VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
      VK_DEBUG_REPORT_ERROR_BIT_EXT |
      VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    vk_createInfo.pfnCallback = debugReportCallback;
    vk_createInfo.pUserData = this;


    VkResult vk_result = vk_CreateDebugReportCallbackEXT(m_vk_instance, &vk_createInfo, nullptr, &vk_debugReportCallback);
    if (vk_result != VK_SUCCESS) {
      return 0x0;
    }
    return vk_debugReportCallback;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData) {
    static const std::map<std::uint32_t, std::string> flagNames{
      {0, ""},
      {VK_DEBUG_REPORT_INFORMATION_BIT_EXT, "VK_DEBUG_REPORT_INFORMATION_BIT_EXT"},
      {VK_DEBUG_REPORT_WARNING_BIT_EXT, "VK_DEBUG_REPORT_WARNING_BIT_EXT"},
      {VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT, "VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT"},
      {VK_DEBUG_REPORT_ERROR_BIT_EXT, "VK_DEBUG_REPORT_ERROR_BIT_EXT"},
      {VK_DEBUG_REPORT_DEBUG_BIT_EXT, "VK_DEBUG_REPORT_DEBUG_BIT_EXT"}
    };
    static const std::vector<std::string> objectTypes{
      "VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_KHR_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_MODE_KHR_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_OBJECT_TABLE_NVX_EXT",
      "VK_DEBUG_REPORT_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX_EXT"
    };

    std::cerr << "Debug report callback:" << std::endl;

    std::cerr << "flags: " <<
      flagNames.at(flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) <<
      flagNames.at(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) <<
      flagNames.at(flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) <<
      flagNames.at(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) <<
      flagNames.at(flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) << std::endl;

    std::cerr << "objectType: " << objectTypes[static_cast<std::size_t>(objectType)] << std::endl;

    std::cerr << "object: 0x" << reinterpret_cast<const void *>(object) << std::endl;

    std::cerr << "location: " << location << std::endl;

    std::cerr << "messageCode: " << messageCode << std::endl;

    std::cerr << "layerPrefix: " << pLayerPrefix << std::endl;

    std::cerr << "message: " << pMessage << std::endl;

    std::cerr << "userData: 0x" << pUserData << std::endl;

    std::cerr << std::endl;

    // The callback returns a VkBool32 that indicates to the calling layer if the Vulkan call should be aborted or not.
    // Applications should always return VK_FALSE so that they see the same behavior with and without validation layers enabled.
    // If the application returns VK_TRUE from it’s callback and the Vulkan call being aborted returns a VkResult,
    // the layer will return VK_ERROR_VALIDATION_FAILED_EXT.
    return VK_FALSE;
  }

  static std::vector<const char *> getEnabledLayerNames() {
    std::vector<const char *> enabledLayerNames;

    if (ENABLE_VALIDATION_LAYERS) {
      for (const auto &layer : validationLayers) {
        enabledLayerNames.push_back(layer.c_str());
      }
    }

    return enabledLayerNames;
  }

  static std::vector<const char *> getEnabledExtensionNames() {
    std::vector<const char *> enabledExtensionNames;

    std::uint32_t glfwExtensionCount;
    const char **glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (std::uint32_t i = 0; i < glfwExtensionCount; i ++) {
      enabledExtensionNames.push_back(glfwExtensionNames[i]);
    }

    if (ENABLE_VALIDATION_LAYERS) {
      enabledExtensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    return enabledExtensionNames;
  }

  static void listInstanceExtensionProperties() {
    std::vector<VkExtensionProperties> extensionProperties = getInstanceExtensionProperties();
    std::cout << "Number of extensions: " << extensionProperties.size() << std::endl;

    for (std::size_t i = 0; i < extensionProperties.size(); i++) {
      std::cout << i << ")" <<
        " extensionName: " << extensionProperties[i].extensionName <<
        " specVersion: " << extensionProperties[i].specVersion << std::endl;
    }
    std::cout << std::endl;
  }

  static std::vector<VkExtensionProperties> getInstanceExtensionProperties() {
    std::uint32_t propertyCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);
    if (propertyCount == 0)
      return std::vector<VkExtensionProperties>();

    std::vector<VkExtensionProperties> properties(propertyCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data());

    return properties;
  }

  static void listInstanceLayerProperties() {
    std::vector<VkLayerProperties> layerProperties = getInstanceLayerProperties();
    std::cout << "Number of layers: " << layerProperties.size() << std::endl;

    for (std::size_t i = 0; i < layerProperties.size(); i++) {
      std::cout << i << ")" <<
        " description: " << layerProperties[i].description <<
        " implementationVersion: " << layerProperties[i].implementationVersion <<
        " layerName: " << layerProperties[i].layerName <<
        " specVersion: " << layerProperties[i].specVersion << std::endl;
    }
    std::cout << std::endl;
  }

  static std::vector<VkLayerProperties> getInstanceLayerProperties() {
    std::uint32_t propertyCount;
    vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
    if (propertyCount == 0)
      return std::vector<VkLayerProperties>();

    std::vector<VkLayerProperties> properties(propertyCount);
    vkEnumerateInstanceLayerProperties(&propertyCount, properties.data());

    return properties;
  }

  static bool checkLayersSupport(const std::vector<std::string> &layerNames) {
    std::vector<VkLayerProperties> layerProperties = getInstanceLayerProperties();

    for (const auto &layerName : layerNames) {
      bool supported = false;
      for (const auto &layerProperty : layerProperties) {
        if (std::strcmp(layerName.c_str(), layerProperty.layerName) == 0) {
          supported = true;
          break;
        }
      }

      if (!supported) {
        return false;
      }
    }

    return true;
  }
};

int main(int argc, char *argv[]) {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    std::system("pause");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
