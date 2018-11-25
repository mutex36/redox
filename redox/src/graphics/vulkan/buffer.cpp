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

#include "graphics.h"
#include "render_system.h"
#include "command_pool.h"
#include "resources\texture.h"

redox::graphics::Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags) :
	_size(size) {
	
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(Graphics::instance().device(), &bufferInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(Graphics::instance().device(), _handle, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	auto memType = Graphics::instance().pick_memory_type(memRequirements.memoryTypeBits, memFlags);
	if (!memType)
		throw Exception("failed to determine memory type");

	allocInfo.memoryTypeIndex = memType.value();

	if (vkAllocateMemory(Graphics::instance().device(), &allocInfo, nullptr, &_memory) != VK_SUCCESS)
		throw Exception("failed to allocate vertex buffer memory");

	vkBindBufferMemory(Graphics::instance().device(), _handle, _memory, 0);
}

redox::graphics::Buffer::~Buffer() {
	vkDestroyBuffer(Graphics::instance().device(), _handle, nullptr);
	vkFreeMemory(Graphics::instance().device(), _memory, nullptr);
}

VkDeviceSize redox::graphics::Buffer::size() const {
	return _size;
}

VkBuffer redox::graphics::Buffer::handle() const {
	return _handle;
}

void redox::graphics::Buffer::map(tl::function_ref<void(void*)> fn) const {
	void* data;
	vkMapMemory(Graphics::instance().device(), _memory, 0, _size, 0, &data);
	fn(data);
	vkUnmapMemory(Graphics::instance().device(), _memory);
}

void redox::graphics::Buffer::copy_to(const Buffer& other) {
	VkBufferCopy copyRegion{};
	copyRegion.size = _size;
	CommandPool::aux_submit([this, &other, &copyRegion](CommandBufferView cbo) {
		vkCmdCopyBuffer(cbo.handle(), _handle, other.handle(), 1, &copyRegion);
	});
}

void redox::graphics::Buffer::copy_to(const Texture& texture) {
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

	CommandPool::aux_submit([this, &texture, &region](CommandBufferView cbo) {
		vkCmdCopyBufferToImage(cbo.handle(), _handle, texture.handle(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	});
}

redox::graphics::StagedBuffer::StagedBuffer(VkDeviceSize size, VkBufferUsageFlags usage) :
	_buffer(size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
	_stagingBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {

}

void redox::graphics::StagedBuffer::upload() {
	_stagingBuffer.copy_to(_buffer);
}

VkBuffer redox::graphics::StagedBuffer::handle() const {
	return _buffer.handle();
}
