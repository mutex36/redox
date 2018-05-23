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
#include "buffer.h"

#include "command_buffer.h"
#include "texture.h"

redox::graphics::Buffer::Buffer(VkDeviceSize size, const Graphics& graphicsRef,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags) :
	_size(size),
	_graphicsRef(graphicsRef) {
	
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(_graphicsRef.device(), &bufferInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_graphicsRef.device(), _handle, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	auto memType = _graphicsRef.pick_memory_type(memRequirements.memoryTypeBits, memFlags);
	if (!memType)
		throw Exception("failed to determine memory type");

	allocInfo.memoryTypeIndex = memType.value();

	if (vkAllocateMemory(_graphicsRef.device(), &allocInfo, nullptr, &_memory) != VK_SUCCESS)
		throw Exception("failed to allocate vertex buffer memory");

	vkBindBufferMemory(_graphicsRef.device(), _handle, _memory, 0);
}

redox::graphics::Buffer::~Buffer() {
	vkDestroyBuffer(_graphicsRef.device(), _handle, nullptr);
	vkFreeMemory(_graphicsRef.device(), _memory, nullptr);
}

VkDeviceSize redox::graphics::Buffer::size() const {
	return _size;
}

VkBuffer redox::graphics::Buffer::handle() const {
	return _handle;
}

void redox::graphics::Buffer::copy_to(const Buffer& other, const CommandBuffer& commandBuffer) {
	VkBufferCopy copyRegion{};
	copyRegion.size = _size;
	vkCmdCopyBuffer(commandBuffer.handle(), _handle, other.handle(), 1, &copyRegion);
}

void redox::graphics::Buffer::copy_to(const Texture& texture, const CommandBuffer& commandBuffer) {
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };

	const auto& ts = texture.dimension();
	region.imageExtent = { ts.width, ts.height, 1 };

	vkCmdCopyBufferToImage(commandBuffer.handle(), _handle, texture.handle(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}