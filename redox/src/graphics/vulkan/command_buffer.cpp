#include "command_buffer.h"

redox::graphics::CommandBuffer::CommandBuffer(VkCommandBuffer handle) :
	_handle(handle) {
}

VkCommandBuffer redox::graphics::CommandBuffer::handle() const {
	return _handle;
}
