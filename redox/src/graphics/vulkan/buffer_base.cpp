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
#include "buffer_base.h"

#include "command_pool.h"
#include "graphics.h"

redox::graphics::BufferBase::BufferBase(VkDeviceSize size, const Graphics& graphicsRef, VkBufferUsageFlags usage)
	: _size(size), _graphicsRef(graphicsRef) {

	_init_buffer(_stagingBuffer, _stagingBufferMemory, _size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	_init_buffer(_handle, _memory, _size, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

redox::graphics::BufferBase::~BufferBase() {
	vkDestroyBuffer(_graphicsRef.device(), _stagingBuffer, nullptr);
	vkFreeMemory(_graphicsRef.device(), _stagingBufferMemory, nullptr);
	vkDestroyBuffer(_graphicsRef.device(), _handle, nullptr);
	vkFreeMemory(_graphicsRef.device(), _memory, nullptr);
}

VkDeviceSize redox::graphics::BufferBase::size() const {
	return _size;
}

VkBuffer redox::graphics::BufferBase::handle() const {
	return _handle;
}

void redox::graphics::BufferBase::transfer(const CommandPool& pool) {
	_copy_buffer(_stagingBuffer, _handle, pool);
}

void redox::graphics::BufferBase::_init_buffer(VkBuffer& handle, VkDeviceMemory& memory, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags) {

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(_graphicsRef.device(), &bufferInfo, nullptr, &handle) != VK_SUCCESS)
		throw Exception("failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_graphicsRef.device(), handle, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	auto memType = _graphicsRef.pick_memory_type(memRequirements.memoryTypeBits, memoryFlags);

	if (!memType)
		throw Exception("failed to determine memory type");

	allocInfo.memoryTypeIndex = memType.value();

	if (vkAllocateMemory(_graphicsRef.device(), &allocInfo, nullptr, &memory) != VK_SUCCESS)
		throw Exception("failed to allocate vertex buffer memory");

	vkBindBufferMemory(_graphicsRef.device(), handle, memory, 0);
}

void redox::graphics::BufferBase::_copy_buffer(VkBuffer source, VkBuffer dest, const CommandPool& pool) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = pool.handle();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(_graphicsRef.device(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = _size;
	vkCmdCopyBuffer(commandBuffer, source, dest, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(_graphicsRef.graphics_queue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(_graphicsRef.graphics_queue());
}
