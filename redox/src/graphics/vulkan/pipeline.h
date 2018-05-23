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
#include "graphics.h"
#include "shader.h"
#include "swapchain.h"
#include "command_pool.h"
#include "command_buffer.h"
#include "core\utility.h"
#include "vertex_layout.h"
#include "render_pass.h"

namespace redox::graphics {
	class Pipeline {
	public:
		Pipeline(const Graphics& graphics, const RenderPass& renderPass, Resource<Shader> vs, Resource<Shader> fs);
		~Pipeline();

		void bind(const CommandBuffer& commandBuffer, const Framebuffer& frameBuffer);
		void unbind(const CommandBuffer& commandBuffer);

		void bind_ubo(const Buffer& ubo);
		void set_viewport(const VkExtent2D& size);

	private:
		void _init();
		void _init_desriptors();
		void _update_viewport(const CommandBuffer& commandBuffer);

		VkPipeline _handle;
		VkPipelineLayout _layout;
		VkExtent2D _viewport;

		VkDescriptorSetLayout _descriptorSetLayout;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSet _descriptorSet;

		Resource<Shader> _vs;
		Resource<Shader> _fs;

		const Graphics& _graphicsRef;
		const RenderPass& _renderPassRef;
	};
}