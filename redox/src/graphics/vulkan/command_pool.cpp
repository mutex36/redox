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
#include "commands.h"

redox::graphics::CommandPool::CommandPool(VkCommandPoolCreateFlags flags) {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = Graphics::instance().queue_family();
	poolInfo.flags = flags; // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(Graphics::instance().device(), &poolInfo, nullptr, &_handle) != VK_SUCCESS) {
		throw Exception("failed to create commandpool");
	}
}

redox::graphics::CommandPool::~CommandPool() {
	vkDestroyCommandPool(Graphics::instance().device(), _handle, nullptr);
}

void redox::graphics::CommandPool::free_all() {
	vkFreeCommandBuffers(Graphics::instance().device(),
		_handle, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
}

void redox::graphics::CommandPool::allocate(uint32_t numBuffers) {
	_commandBuffers.resize(numBuffers);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = _handle;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

	if (vkAllocateCommandBuffers(Graphics::instance().device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
		throw Exception("failed to create commandbuffers");
	}
}

redox::graphics::CommandBufferView redox::graphics::CommandPool::operator[](std::size_t index) const {
	return _commandBuffers[index];
}

redox::graphics::CommandBufferView::CommandBufferView(VkCommandBuffer handle) :
	_handle(handle) {
}

//void redox::graphics::CommandBufferView::_flush() {
//	std::sort(_commands.begin(), _commands.end(), [](const auto& a, const auto& b) {
//		return a->sort_key() < b->sort_key();
//	});
//
//	for (auto& command : _commands) {
//		command->execute(*this);
//	}
//}

void redox::graphics::CommandBufferView::submit(UniquePtr<ICommand> command) {
	//_commands.push_back(std::move(command));
	command->execute(*this);
}

void redox::graphics::CommandBufferView::begin_record() {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	if (vkBeginCommandBuffer(_handle, &beginInfo) != VK_SUCCESS) {
		throw Exception("failed to begin recording of commandBuffer");
	}
}

void redox::graphics::CommandBufferView::end_record() {
	//_flush();

	if (vkEndCommandBuffer(_handle) != VK_SUCCESS) {
		throw Exception("failed to end recording of commandBuffer");
	}
}

VkCommandBuffer redox::graphics::CommandBufferView::handle() const {
	return _handle;
}

redox::graphics::AuxCommandPool::AuxCommandPool()
	: CommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) {

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkCreateFence(Graphics::instance().device(), &fenceInfo, VK_NULL_HANDLE, &_queueFence);
}

redox::graphics::AuxCommandPool::~AuxCommandPool() {
	vkDestroyFence(Graphics::instance().device(), _queueFence, VK_NULL_HANDLE);
}

void redox::graphics::AuxCommandPool::submit(FunctionRef<void(const CommandBufferView&)> fn,
	bool sync, uint64_t timeout) const noexcept {

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = _handle;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(Graphics::instance().device(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	fn(CommandBufferView{ commandBuffer });
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(Graphics::instance().graphics_queue(), 1, &submitInfo, sync ? _queueFence : NULL);
	if (sync) {
		vkWaitForFences(Graphics::instance().device(), 1, &_queueFence, VK_TRUE, timeout);
		vkResetFences(Graphics::instance().device(), 1, &_queueFence);
	}
	vkFreeCommandBuffers(Graphics::instance().device(), _handle, 1, &commandBuffer);
}
