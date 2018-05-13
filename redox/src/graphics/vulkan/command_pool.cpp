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
#include "command_pool.h"
#include "graphics.h"

redox::graphics::CommandPool::CommandPool(const Graphics& graphics, VkCommandPoolCreateFlags flags)
	: _graphicsRef(graphics) {
	_init(flags);
}

redox::graphics::CommandPool::~CommandPool() {
	vkDestroyCommandPool(_graphicsRef.device(), _handle, nullptr);
}

void redox::graphics::CommandPool::free_all() {
	vkFreeCommandBuffers(_graphicsRef.device(), 
		_handle, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
}

void redox::graphics::CommandPool::resize(std::size_t numBuffers) {
	_commandBuffers.resize(numBuffers);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = _handle;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

	if (vkAllocateCommandBuffers(_graphicsRef.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
		throw Exception("failed to create commandbuffers");
}

std::size_t redox::graphics::CommandPool::size() const {
	return _commandBuffers.size();
}

VkCommandPool redox::graphics::CommandPool::handle() const {
	return _handle;
}

VkCommandBuffer redox::graphics::CommandPool::operator[](std::size_t index) const {
	return _commandBuffers[index];
}

void redox::graphics::CommandPool::_init(VkCommandPoolCreateFlags flags) {

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = _graphicsRef.queue_family();
	poolInfo.flags = flags; // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(_graphicsRef.device(), &poolInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create commandpool");
}
