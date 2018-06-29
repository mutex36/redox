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

#include "core\non_copyable.h"
#include "core\utility.h"
#include "core\error.h"

namespace redox::graphics {
	class CommandBuffer;
	class Texture;

	class Buffer : public NonCopyable {
	public:
		Buffer(VkDeviceSize size, const Graphics& graphicsRef, 
			VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags);
		~Buffer();

		VkDeviceSize size() const;
		VkBuffer handle() const;

		template<class Fn>
		void map(Fn&& fn) const {
			void* data;
			vkMapMemory(_graphicsRef.device(), _memory, 0, _size, 0, &data);
			fn(data);
			vkUnmapMemory(_graphicsRef.device(), _memory);
		}

		void copy_to(const Buffer& other, const CommandBuffer& commandBuffer);
		void copy_to(const Texture& texture, const CommandBuffer& commandBuffer);

	protected:
		VkBuffer _handle;
		VkDeviceMemory _memory;
		VkDeviceSize _size;

		const Graphics& _graphicsRef;
	};

	class StagedBuffer : public NonCopyable {
	public:
		StagedBuffer(VkDeviceSize size, const Graphics& graphics, VkBufferUsageFlags usage);
		~StagedBuffer() = default;

		template<class Fn>
		void map(Fn&& fn) {
			_stagingBuffer.map(std::forward<Fn>(fn));
		}

		void upload(const CommandBuffer& commandBuffer);
		VkBuffer handle() const;

	private:
		Buffer _buffer;
		Buffer _stagingBuffer;
	};

	struct UniformBuffer : public StagedBuffer {
		UniformBuffer(VkDeviceSize size, const Graphics& graphics) :
			StagedBuffer(size, graphics, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
		}
	};

	struct VertexBuffer : public StagedBuffer {
		VertexBuffer(VkDeviceSize size, const Graphics& graphics) :
			StagedBuffer(size, graphics, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
		}
	};

	struct IndexBuffer : public StagedBuffer {
		IndexBuffer(VkDeviceSize size, const Graphics& graphics) :
			StagedBuffer(size, graphics, VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
		}
	};
}