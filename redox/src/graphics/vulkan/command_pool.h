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

namespace redox::graphics {
	class Graphics;

	struct IndexedDraw {
		Resource<Mesh> mesh;
		Resource<Material> material;
		uint32_t vertexOffset;
		uint32_t vertexCount;
	};

	class CommandBuffer : public NonCopyable {
	public:
		CommandBuffer(VkCommandBuffer handle);
		~CommandBuffer() = default;

		void submit(const IndexedDraw& command) const;

		template<class Fn>
		void record(Fn&& fn) const {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			if (vkBeginCommandBuffer(_handle, &beginInfo) != VK_SUCCESS)
				throw Exception("failed to begin recording of commandBuffer");

			fn();

			if (vkEndCommandBuffer(_handle) != VK_SUCCESS)
				throw Exception("failed to end recording of commandBuffer");
		}

		VkCommandBuffer handle() const;

	private:
		VkCommandBuffer _handle;
	};

	class CommandPool {
	public:
		CommandPool(const Graphics& graphics,
			VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		~CommandPool();

		void free_all();
		void allocate(uint32_t numBuffers);

		template<class Fn>
		void quick_submit(Fn&& fn) const {
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = _handle;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(_graphicsRef.device(), &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
			fn(CommandBuffer{ commandBuffer });
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(_graphicsRef.graphics_queue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(_graphicsRef.graphics_queue());
			vkFreeCommandBuffers(_graphicsRef.device(), _handle, 1, &commandBuffer);
		}

		CommandBuffer operator[](std::size_t index) const;

	private:
		void _init(VkCommandPoolCreateFlags flags);

		VkCommandPool _handle;
		redox::Buffer<VkCommandBuffer> _commandBuffers;

		const Graphics& _graphicsRef;
	};
}