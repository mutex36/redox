#include "render_system.h"

#include "core\buffer.h"
#include "core\string.h"

#include "core\logging\log.h"
#define RDX_LOG_TAG "Vulkan"

redox::RenderSystem::RenderSystem(const Window* window) : _window(window) {
	_init_instance();
	_init_phsyical_device();
	_init_surface();
	_init_device();
}

redox::RenderSystem::~RenderSystem() {
	vkDestroyDevice(_device, nullptr);
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyInstance(_instance, NULL);
}

static VkBool32 DebugMessageCallback(
	VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
	size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData) {

	RDX_LOG("Vulkan: Debug Message ({0}): {1}", msgCode, pMsg);
	RDX_ASSERT_FALSE(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT);

	return VK_FALSE;
}

void redox::RenderSystem::_init_instance() {

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Test";
	appInfo.pEngineName = "RedoxEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

#ifdef RDX_DEBUG
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	Buffer<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	for (auto& layer : availableLayers)
		RDX_LOG("Layer: {0}", layer.layerName);
#endif

#ifdef RDX_VULKAN_VALIDATION
	_layers.push("VK_LAYER_LUNARG_standard_validation");
#endif

	_extensions.push(VK_KHR_SURFACE_EXTENSION_NAME);

#ifdef RDX_PLATFORM_WINDOWS
	_extensions.push(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef RDX_VULKAN_VALIDATION
	_extensions.push(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = _extensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = _extensions.data();
	instanceCreateInfo.enabledLayerCount = _layers.size();
	instanceCreateInfo.ppEnabledLayerNames = _layers.data();

	RDX_ASSERT_EQ(
		vkCreateInstance(&instanceCreateInfo, NULL, &_instance), VK_SUCCESS);

#ifdef RDX_VULKAN_VALIDATION
	auto vkCreateDebugReportCallbackEXT = 
		(PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");

	if (vkCreateDebugReportCallbackEXT) {
		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
		dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)DebugMessageCallback;
		dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	
		RDX_ASSERT_EQ(vkCreateDebugReportCallbackEXT(
			_instance, reinterpret_cast<const VkDebugReportCallbackCreateInfoEXT*>(&dbgCreateInfo),
			nullptr, &_debugreportcallback), VK_SUCCESS);
	}
#endif
}

void redox::RenderSystem::_init_phsyical_device() {

	uint32_t num;
	RDX_ASSERT_EQ(vkEnumeratePhysicalDevices(_instance, &num, NULL), VK_SUCCESS);
	RDX_ASSERT_NEQ(num, 0);

	Buffer<VkPhysicalDevice> devices(num);
	RDX_ASSERT_EQ(vkEnumeratePhysicalDevices(_instance, &num, devices.data()), VK_SUCCESS);

	for (auto& dev : devices) {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(dev, &deviceProperties);
		vkGetPhysicalDeviceFeatures(dev, &deviceFeatures);
 
		RDX_LOG("Physical device: {0}", deviceProperties.deviceName);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			_physical_device = dev;
			break;
		}
	}

	RDX_ASSERT_TRUE(_physical_device);

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queueFamilyCount, nullptr);
	_queue_families.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queueFamilyCount, _queue_families.data());
}

void redox::RenderSystem::_init_device() {
	_graphics_queue_family = _find_queue_family([](auto& qfm, auto index) {
		return qfm.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	});

	Buffer<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriority = 1.0f;
	queueCreateInfos.reserve(2);

	VkDeviceQueueCreateInfo graphicsQueueInfo{};
	graphicsQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueInfo.queueFamilyIndex = _graphics_queue_family;
	graphicsQueueInfo.queueCount = 1;
	graphicsQueueInfo.pQueuePriorities = &queuePriority;
	queueCreateInfos.push(graphicsQueueInfo);

	VkDeviceQueueCreateInfo presentationQueueInfo{};
	presentationQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	presentationQueueInfo.queueFamilyIndex = _presentation_queue_family;
	presentationQueueInfo.queueCount = 1;
	presentationQueueInfo.pQueuePriorities = &queuePriority;
	queueCreateInfos.push(presentationQueueInfo);

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;

	_device_extensions.push(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	createInfo.enabledExtensionCount = _device_extensions.size();
	createInfo.ppEnabledExtensionNames = _device_extensions.data();

#ifdef RDX_VULKAN_VALIDATION
	createInfo.enabledLayerCount = static_cast<uint32_t>(_layers.size());
	createInfo.ppEnabledLayerNames = _layers.data();
#else
	createInfo.enabledLayerCount = 0;
#endif

	RDX_ASSERT_EQ(
		vkCreateDevice(_physical_device, &createInfo, nullptr, &_device), VK_SUCCESS);


	vkGetDeviceQueue(_device, _presentation_queue_family, 0, &_presentation_queue);
	RDX_ASSERT(_presentation_queue);
}

void redox::RenderSystem::_init_surface() {

#ifdef RDX_PLATFORM_WINDOWS
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;

	createInfo.hwnd = reinterpret_cast<HWND>(_window->get("hwnd"));
	createInfo.hinstance = reinterpret_cast<HINSTANCE>(_window->get("hinstance"));

	auto CreateWin32SurfaceKHR = 
		reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(_instance, "vkCreateWin32SurfaceKHR"));

	RDX_ASSERT(CreateWin32SurfaceKHR);
	RDX_ASSERT_EQ(
		CreateWin32SurfaceKHR(_instance, &createInfo, nullptr, &_surface), VK_SUCCESS);

#endif

	_presentation_queue_family = _find_queue_family([this](auto& qfm, auto index) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device, index, _surface, &presentSupport);
		return presentSupport;
	});
}