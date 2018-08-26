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

#include "descriptor_pool.h"
#include "command_pool.h"
#include "pipeline_cache.h"
#include "render_pass.h"
#include "swapchain.h"

#include "factory/model_factory.h"
#include "factory/shader_factory.h"
#include "factory/texture_factory.h"

#include <optional> //std::optional

namespace redox::graphics {
	class Graphics {
	public:
		static Graphics* instance;

		Graphics(const platform::Window& window);
		~Graphics();

		VkDevice device() const;
		VkPhysicalDevice physical_device() const;
		VkSurfaceKHR surface() const;
		VkQueue graphics_queue() const;
		VkQueue present_queue() const;
		uint32_t queue_family() const;

		void present() const;
		void wait_pending() const;

		std::optional<uint32_t> pick_memory_type(
			uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

		VkPresentModeKHR pick_presentation_mode() const;
		VkSurfaceFormatKHR pick_surface_format() const;

		const CommandPool& aux_command_pool() const;
		const DescriptorPool& descriptor_pool() const;
		const PipelineCache& pipeline_cache() const;
		const RenderPass& forward_render_pass() const;
		const Swapchain& swap_chain() const;

	private:
		void _swapchain_event_create();

		void _init_instance();
		void _init_physical_device();
		void _init_surface(const platform::Window& window);
		void _init_device();

		std::optional<VkPhysicalDevice> _pick_device();
		std::optional<uint32_t> _pick_queue_family();

		static_instance_wrapper _iw{ this };

		RenderPass _forwardRenderPass;
		Swapchain _swapchain;

		ModelFactory _modelFactory;
		TextureFactory _textureFactory;
		ShaderFactory  _shaderFactory;

		CommandPool _auxCommandPool;
		DescriptorPool _descriptorPool;
		PipelineCache _pipelineCache;

		uint32_t _queueFamily;
		VkQueue _graphicsQueue;

		VkInstance _instance;
		VkDevice _device;
		VkPhysicalDevice _physicalDevice;
		VkSurfaceKHR _surface;

#ifdef RDX_VULKAN_VALIDATION
		VkDebugReportCallbackEXT _debugReportCallback;
#endif
	};
}