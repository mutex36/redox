#pragma once
#include "core\core.h"
#include <vulkan\vulkan.h>

#ifdef RDX_PLATFORM_WINDOWS 
#include "core\sys\windows.h"
#include <vulkan\vulkan_win32.h>
#endif

#define RDX_VULKAN_VALIDATION 1

namespace redox::graphics {

	class RenderSystem {
	public:
		RenderSystem();
		~RenderSystem();


	private:
		void _init_instance();
		void _init_device(VkPhysicalDevice device);

		VkInstance _instance;
		VkPhysicalDevice _device;
		
#ifdef RDX_VULKAN_VALIDATION
		VkDebugReportCallbackEXT _debugreportcallback;
#endif

	};
}