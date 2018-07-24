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
#include "core\buffer.h"
#include "vulkan.h"

#include "resources/mesh.h"
#include "resources/material.h"

#include <thirdparty/function_ref/function_ref.hpp>

namespace redox::graphics {
	class Graphics;

	struct VertexRange {
		uint32_t start, end;
	};

	struct IndexedDraw {
		ResourceHandle<Mesh> mesh;
		ResourceHandle<Material> material;
		VertexRange range;
	};

	class CommandBufferView {
	public:
		CommandBufferView(VkCommandBuffer handle);
		~CommandBufferView() = default;

		void submit(const IndexedDraw& command) const;
		void record(tl::function_ref<void()> fn) const;

		VkCommandBuffer handle() const;

	private:
		VkCommandBuffer _handle;
	};

	class CommandPool : public NonCopyable {
	public:
		CommandPool(VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		~CommandPool();

		void free_all();
		void allocate(uint32_t numBuffers);
		void quick_submit(tl::function_ref<void(const CommandBufferView&)> fn) const;

		CommandBufferView operator[](std::size_t index) const;

	private:
		VkCommandPool _handle;
		redox::Buffer<VkCommandBuffer> _commandBuffers;
	};
}