#include "staged_buffer.h"
#include "command_buffer.h"

redox::graphics::StagedBuffer::StagedBuffer(VkDeviceSize size, const Graphics& graphics, VkBufferUsageFlags usage) :
_buffer(size, graphics, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
_stagingBuffer(size, graphics, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {

}

void redox::graphics::StagedBuffer::upload(const CommandBuffer& commandBuffer) {
	_stagingBuffer.copy_to(_buffer, commandBuffer);
}

const redox::graphics::Buffer& redox::graphics::StagedBuffer::main_buffer() const {
	return _buffer;
}

const redox::graphics::Buffer& redox::graphics::StagedBuffer::staging_buffer() const {
	return _stagingBuffer;
}