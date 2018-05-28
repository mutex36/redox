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
#include "texture.h"

#include "vertex_layout.h"
#include "render_pass.h"

#include "core\utility.h"
#include <functional> //std::function

namespace redox::graphics {
	class CommandPool;
	class CommandBuffer;

	class Pipeline {
	public:
		Pipeline(const Graphics& graphics, 
			const RenderPass& renderPass, const VertexLayout& vLayout,
			Resource<Shader> vs, Resource<Shader> fs);
		~Pipeline();

		void bind(const CommandBuffer& commandBuffer, const Framebuffer& frameBuffer);
		void unbind(const CommandBuffer& commandBuffer);

		void set_viewport(const VkExtent2D& size);

		void bind_resource(const Texture& texture, uint32_t bindingPoint);
		void bind_resource(const UniformBuffer& ubo, uint32_t bindingPoint);

	private:
		void _init(const VertexLayout& vLayout);
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