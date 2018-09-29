/*
redox
-----------
MIT License

Copyright (c) 2018 Luis von der Eltz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "graphics.h"
#include "core\utility.h"

#include "core\application.h"

const redox::graphics::Graphics& redox::graphics::Graphics::instance() {
	return Application::instance->render_system().graphics();
}

redox::graphics::Graphics::Graphics(const platform::Window& window) {
	_init_instance();
	_init_physical_device();
	_init_surface(window);
	_init_device();

	ResourceManager::instance().register_factory(&_textureFactory);
	ResourceManager::instance().register_factory(&_modelFactory);
	ResourceManager::instance().register_factory(&_shaderFactory);
}

redox::graphics::Graphics::~Graphics() {
	vkDestroySurfaceKHR(_instance, _surface, nullptr);

#ifdef RDX_VULKAN_VALIDATION
	auto vkDestroyDebugReportCallbackEXT =
		(PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT");
	vkDestroyDebugReportCallbackEXT(_instance, _debugReportCallback, nullptr);
#endif

	vkDestroyDevice(_device, nullptr);
	vkDestroyInstance(_instance, nullptr);
}

VkDevice redox::graphics::Graphics::device() const {
	return _device;
}

VkPhysicalDevice redox::graphics::Graphics::physical_device() const {
	return _physicalDevice;
}

VkSurfaceKHR redox::graphics::Graphics::surface() const {
	return _surface;
}

VkQueue redox::graphics::Graphics::graphics_queue() const {
	return _graphicsQueue;
}

VkQueue redox::graphics::Graphics::present_queue() const {
	return _graphicsQueue;
}

uint32_t redox::graphics::Graphics::queue_family() const {
	return _queueFamily;
}

void redox::graphics::Graphics::wait_pending() const {
	vkDeviceWaitIdle(_device);
}

std::optional<uint32_t> redox::graphics::Graphics::pick_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

	for (std::size_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return static_cast<uint32_t>(i);
		}
	}
	return std::nullopt;
}

VkPresentModeKHR redox::graphics::Graphics::pick_presentation_mode() const {
	uint32_t count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &count, nullptr);
	redox::Buffer<VkPresentModeKHR> modes(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &count, modes.data());

	if (Application::instance->config().get("Engine", "vsync"))
		return VK_PRESENT_MODE_FIFO_KHR;

	for (auto& mode : modes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			return mode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR redox::graphics::Graphics::pick_surface_format() const {
	uint32_t count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &count, nullptr);
	redox::Buffer<VkSurfaceFormatKHR> formats(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &count, formats.data());

	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (auto& format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	return formats[0];
}

void redox::graphics::Graphics::_init_instance() {
	RDX_LOG("Initializing instance...");

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "RedoxApp";
	appInfo.pEngineName = "RedoxEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

#ifdef RDX_DEBUG
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	redox::Buffer<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	for (auto& layer : availableLayers)
		RDX_LOG("Layer: {0}", layer.layerName);
#endif

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = util::array_size<uint32_t>(RDX_VULKAN_EXTENSIONS);
	instanceCreateInfo.ppEnabledExtensionNames = RDX_VULKAN_EXTENSIONS;
	instanceCreateInfo.enabledLayerCount = util::array_size<uint32_t>(RDX_VULKAN_LAYERS);
	instanceCreateInfo.ppEnabledLayerNames = RDX_VULKAN_LAYERS;

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &_instance) != VK_SUCCESS)
		throw Exception("failed to create graphics instance");

#ifdef RDX_VULKAN_VALIDATION
	auto vkCreateDebugReportCallbackEXT =
		(PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");

	if (vkCreateDebugReportCallbackEXT) {
		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
		dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)DebugMessageCallback;
		dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
			VK_DEBUG_REPORT_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_DEBUG_BIT_EXT |
			VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

		if (vkCreateDebugReportCallbackEXT(
			_instance, &dbgCreateInfo, nullptr, &_debugReportCallback) != VK_SUCCESS)
				throw Exception("failed to create debug report");

	}
#endif
}

void redox::graphics::Graphics::_init_physical_device() {
	RDX_LOG("Choosing physical device...");

	auto device = _pick_device();
	if (!device)
		throw Exception("No suitable physical device found");

	_physicalDevice = device.value();
}

void redox::graphics::Graphics::_init_surface(const platform::Window& window) {

#ifdef RDX_PLATFORM_WINDOWS
	RDX_LOG("Creating surface (WIN32)...");

	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = reinterpret_cast<HWND>(window.native_handle());
	createInfo.hinstance = GetModuleHandle(0);

	auto CreateWin32SurfaceKHR =
		reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(_instance, "vkCreateWin32SurfaceKHR"));

	if (!CreateWin32SurfaceKHR ||
		CreateWin32SurfaceKHR(_instance, &createInfo, nullptr, &_surface) != VK_SUCCESS)
			throw Exception("failed to create window surface (WIN32)");

#endif
}

void redox::graphics::Graphics::_init_device() {
	RDX_LOG("Initializing logical device...");

	auto fqi = _pick_queue_family();
	if (!fqi) throw Exception("could not find suitable queue family");
	_queueFamily = fqi.value();

	float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo queueInfo{};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.queueFamilyIndex = _queueFamily;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = util::array_size<uint32_t>(RDX_VULKAN_DEVICE_EXTENSIONS);
	createInfo.ppEnabledExtensionNames = RDX_VULKAN_DEVICE_EXTENSIONS;

#ifdef RDX_VULKAN_VALIDATION
	createInfo.enabledLayerCount = util::array_size<uint32_t>(RDX_VULKAN_LAYERS);
	createInfo.ppEnabledLayerNames = RDX_VULKAN_LAYERS;
#endif

	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
		throw Exception("failed to create logical device");
	
	vkGetDeviceQueue(_device, _queueFamily, 0, &_graphicsQueue);
	//vkGetDeviceQueue(_device, _queueIndex, 0, &_presentQueue);
}

std::optional<VkPhysicalDevice> redox::graphics::Graphics::_pick_device() {
	uint32_t num;
	vkEnumeratePhysicalDevices(_instance, &num, nullptr);

	redox::Buffer<VkPhysicalDevice> devices(num);
	vkEnumeratePhysicalDevices(_instance, &num, devices.data());

	for (auto& dev : devices) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(dev, &deviceProperties);

		//VkPhysicalDeviceFeatures deviceFeatures;
		//vkGetPhysicalDeviceFeatures(dev, &deviceFeatures);

		RDX_LOG("Physical device: {0}", deviceProperties.deviceName);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return dev;
	}

	return std::nullopt;
}

std::optional<uint32_t> redox::graphics::Graphics::_pick_queue_family() {

	redox::Buffer<VkQueueFamilyProperties> queueFamilies;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);
	queueFamilies.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.data());

	for (size_t queueIndex = 0; queueIndex < queueFamilies.size(); queueIndex++) {
		if (queueFamilies[queueIndex].queueCount < 2)
			continue;

		if (!(queueFamilies[queueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			continue;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, 
			static_cast<uint32_t>(queueIndex), _surface, &presentSupport);

		if (!presentSupport) continue;
		
		return static_cast<uint32_t>(queueIndex);
	}

	return std::nullopt;
}
