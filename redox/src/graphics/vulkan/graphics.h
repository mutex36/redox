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
#include "resources\resource_factory.h"
#include "shader.h"

#include <optional> //std::optional

namespace redox {
	class Graphics {
	public:
		Graphics(const Window& window);
		~Graphics();

		VkDevice device() const;
		VkPhysicalDevice physical_device() const;
		VkSurfaceKHR surface() const;
		VkQueue graphics_queue() const;
		VkQueue present_queue() const;
		uint32_t queue_family() const;

		ResourceFactory<Shader>& get_shader_factory();

	private:
		void _init_instance();
		void _init_physical_device();
		void _init_surface();
		void _init_device();

		VkPhysicalDevice _pick_device();
		std::optional<uint32_t> _pick_queue_family();

		ResourceFactory<Shader> _shaderFactory;

		Buffer<VkQueueFamilyProperties> _queueFamilies;
		uint32_t _queueFamily;
		VkQueue _graphicsQueue;

		VkInstance _instance;
		VkDevice _device;
		VkPhysicalDevice _physicalDevice;
		VkSurfaceKHR _surface;

		const Window& _window;

#ifdef RDX_VULKAN_VALIDATION
		VkDebugReportCallbackEXT _debugReportCallback;
#endif
	};
}