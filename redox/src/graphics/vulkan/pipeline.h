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
#include "sampler.h"
#include "buffer.h"

#include "vertex_layout.h"
#include "descriptor_layout.h"

#include "resources\shader.h"
#include "resources\texture.h"

#include "core\utility.h"
#include <functional> //std::function

namespace redox::graphics {
	class CommandPool;
	class CommandBuffer;
	class RenderPass;
	class Framebuffer;

	class Pipeline {
	public:
		Pipeline(const RenderPass& renderPass, 
			const VertexLayout& vLayout, const DescriptorLayout& dLayout,
			ResourceHandle<Shader> vs, ResourceHandle<Shader> fs);
		~Pipeline();

		void bind(const CommandBuffer& commandBuffer);
		void set_viewport(const VkExtent2D& size);

		VkPipelineLayout layout() const;
		VkDescriptorSetLayout descriptorLayout() const;

	private:
		void _init(const VertexLayout& vLayout, const RenderPass& renderPass);
		void _init_desriptors(const DescriptorLayout& dLayout);
		void _update_viewport(const CommandBuffer& cbo);

		VkPipeline _handle;
		VkPipelineLayout _layout;
		VkExtent2D _viewport;

		VkDescriptorSetLayout _descriptorSetLayout;
		ResourceHandle<Shader> _vs;
		ResourceHandle<Shader> _fs;
	};
}