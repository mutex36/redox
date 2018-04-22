#pragma once
#include "vulkan.h"
#include "core\core.h"
#include "core\buffer.h"
#include "core\non_copyable.h"

#include "window\window.h"

#include <type_traits> //std::forward

namespace redox {

	class RenderSystem : public NonCopyable {
	public:
		RenderSystem(const Window* window);
		~RenderSystem();

	private:
		void _init_instance();
		void _init_phsyical_device();
		void _init_device();
		void _init_surface();

		VkInstance _instance{ nullptr };
		VkPhysicalDevice _physical_device{ nullptr };
		VkDevice _device{ nullptr };
		VkSurfaceKHR _surface{ nullptr };

		Buffer<const char*> _layers;
		Buffer<const char*> _extensions;
		Buffer<const char*> _device_extensions;


		int32_t _graphics_queue_family{ -1 };
		int32_t _presentation_queue_family{ -1 };

		VkQueue _presentation_queue{ nullptr };

		Buffer<VkQueueFamilyProperties> _queue_families;

		template<class Fn>
		int32_t _find_queue_family(Fn&& fn) {
			for (int32_t i = 0; i < _queue_families.size(); i++) {
				if (_queue_families[i].queueCount > 0 && fn(_queue_families[i], i))
					return i;
			}
			return -1;
		}

		const Window* _window;

#ifdef RDX_VULKAN_VALIDATION
		VkDebugReportCallbackEXT _debugreportcallback{ nullptr };
#endif

	};
}