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

redox::graphics::CommandPool::CommandPool(VkCommandPoolCreateFlags flags) {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = Graphics::instance().queue_family();
	poolInfo.flags = flags; // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(Graphics::instance().device(), &poolInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create commandpool");
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

	if (vkAllocateCommandBuffers(Graphics::instance().device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
		throw Exception("failed to create commandbuffers");
}

void redox::graphics::CommandPool::aux_submit(FunctionRef<void(const CommandBufferView&)> fn) {

	//TODO: make it live somewhere...
	CommandPool cp(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = cp._handle;
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

	VkFence fence;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkCreateFence(Graphics::instance().device(), &fenceInfo, VK_NULL_HANDLE, &fence);

	vkQueueSubmit(Graphics::instance().graphics_queue(), 1, &submitInfo, fence);
	vkWaitForFences(Graphics::instance().device(), 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkFreeCommandBuffers(Graphics::instance().device(), cp._handle, 1, &commandBuffer);
	vkDestroyFence(Graphics::instance().device(), fence, VK_NULL_HANDLE);
}

redox::graphics::CommandBufferView redox::graphics::CommandPool::operator[](std::size_t index) const {
	return _commandBuffers[index];
}

redox::graphics::CommandBufferView::CommandBufferView(VkCommandBuffer handle) :
	_handle(handle) {
}

void redox::graphics::CommandBufferView::submit(const IndexedDraw& command) const {
	command.material->bind(_handle);
	command.mesh->bind(_handle);
	vkCmdDrawIndexed(_handle, command.range.count, 1, command.range.start, 0, 0);
}

void redox::graphics::CommandBufferView::begin_record() const {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	if (vkBeginCommandBuffer(_handle, &beginInfo) != VK_SUCCESS) {
		throw Exception("failed to begin recording of commandBuffer");
	}
}

void redox::graphics::CommandBufferView::end_record() const {
	if (vkEndCommandBuffer(_handle) != VK_SUCCESS) {
		throw Exception("failed to end recording of commandBuffer");
	}
}

VkCommandBuffer redox::graphics::CommandBufferView::handle() const {
	return _handle;
}
