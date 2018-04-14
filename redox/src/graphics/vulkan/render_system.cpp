#include "render_system.h"

#include "core\buffer.h"
#include "core\string.h"
#include "core\logging\log.h"

redox::graphics::RenderSystem::RenderSystem() 
	: _debugreportcallback(nullptr) {
	
	_init_instance();

	uint32_t num;
	RDX_ASSERT_EQ(vkEnumeratePhysicalDevices(_instance, &num, NULL), VK_SUCCESS);

	Buffer<VkPhysicalDevice> devices;
	devices.reserve(num);
	RDX_ASSERT_EQ(vkEnumeratePhysicalDevices(_instance, &num, devices.data()), VK_SUCCESS);

	if (num == 0)
		throw Exception("no physical device found.");

	_init_device(devices[0]);

}

redox::graphics::RenderSystem::~RenderSystem() {
}


static VkBool32 DebugMessageCallback(
	VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
	size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData) {


	RDX_LOG("Vulkan: Debug Message ({0}): {1}", msgCode, pMsg);
	RDX_ASSERT_FALSE(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT);

	return VK_FALSE;
}


void redox::graphics::RenderSystem::_init_instance() {

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Test";
	appInfo.pEngineName = "RedoxEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	Buffer<const char*> extensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,

#ifdef RDX_PLATFORM_WINDOWS
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif

#ifdef RDX_VULKAN_VALIDATION
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
	};

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = extensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

#if RDX_VULKAN_VALIDATION
	instanceCreateInfo.enabledLayerCount = 1;
	const char *validationLayerNames[] = { "VK_LAYER_LUNARG_standard_validation" };
	instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
#endif

	RDX_ASSERT_EQ(
		vkCreateInstance(&instanceCreateInfo, NULL, &_instance), VK_SUCCESS);

#if RDX_VULKAN_VALIDATION
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

void redox::graphics::RenderSystem::_init_device(VkPhysicalDevice device) {

	
}
