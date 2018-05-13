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
#include "core\error.h"

namespace redox::graphics {
	class Graphics;
	class CommandPool;

	class BufferBase{
	public:
		BufferBase(VkDeviceSize size, const Graphics& graphicsRef, VkBufferUsageFlags usage);
		~BufferBase();

		VkDeviceSize size() const;
		VkBuffer handle() const;
		void transfer(const CommandPool& pool);

		template<class Fn>
		void map(Fn&& fn) const {
			void* data;
			vkMapMemory(_graphicsRef.device(), _stagingBufferMemory, 0, _size, 0, &data);
			fn(data);
			vkUnmapMemory(_graphicsRef.device(), _stagingBufferMemory);
		}

	private:
		VkBuffer _stagingBuffer;
		VkDeviceMemory _stagingBufferMemory;

		VkBuffer _handle;
		VkDeviceMemory _memory;
		VkDeviceSize _size;

		void _init_buffer(VkBuffer& handle, VkDeviceMemory& memory, VkDeviceSize size,
			VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags);
		void _copy_buffer(VkBuffer source, VkBuffer dest, const CommandPool& pool);

		const Graphics& _graphicsRef;
	};
}