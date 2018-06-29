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
#pragma once
#include "vulkan.h"
#include "platform\window.h"
#include "core\config\config.h"

#include <optional> //std::optional

namespace redox::graphics {
	class Graphics {
	public:
		Graphics(const platform::Window& window, const Configuration& config);
		~Graphics();

		VkDevice device() const;
		VkPhysicalDevice physical_device() const;
		VkSurfaceKHR surface() const;
		VkQueue graphics_queue() const;
		VkQueue present_queue() const;
		uint32_t queue_family() const;

		void wait_pending() const;

		std::optional<uint32_t> pick_memory_type(
			uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

		VkPresentModeKHR pick_presentation_mode() const;
		VkSurfaceFormatKHR pick_surface_format() const;

	private:
		void _init_instance();
		void _init_physical_device();
		void _init_surface(const platform::Window& window);
		void _init_device();

		std::optional<VkPhysicalDevice> _pick_device();
		std::optional<uint32_t> _pick_queue_family();

		uint32_t _queueFamily;
		VkQueue _graphicsQueue;

		VkInstance _instance;
		VkDevice _device;
		VkPhysicalDevice _physicalDevice;
		VkSurfaceKHR _surface;

		const Configuration& _configRef;

#ifdef RDX_VULKAN_VALIDATION
		VkDebugReportCallbackEXT _debugReportCallback;
#endif
	};
}