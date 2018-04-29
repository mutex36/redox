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
#include "render_system.h"

#include "core\buffer.h"
#include "core\string.h"
#include "core\utility.h"

#include "core\profiling\profiler.h"

#include "core\logging\log.h"
#define RDX_LOG_TAG "Vulkan"

#include "shader.h"

redox::RenderSystem::RenderSystem(const Window& window)
	: _graphics(window), _swapchain(_graphics),
	_pipeline(_graphics, _swapchain),
	_commandPool(_graphics, _swapchain.size()) {

	_init_semaphores();
}

redox::RenderSystem::~RenderSystem() {
	vkDestroySemaphore(_graphics.device(), _renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(_graphics.device(), _imageAvailableSemaphore, nullptr);
}

void redox::RenderSystem::demo_setup() {
	_RDX_PROFILE;
	for (std::size_t index = 0; index < _commandPool.size(); ++index) {
		auto commandBuffer = _commandPool[index];

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw Exception("failed to begin recording of commandBuffer");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _pipeline.render_pass();
		renderPassInfo.framebuffer = _pipeline[index];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = _swapchain.extent();

		VkClearValue clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.handle());
		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw Exception("failed to end recording of commandBuffer");
	}
}

void redox::RenderSystem::render() {
	vkQueueWaitIdle(_graphics.present_queue());
	auto swapChainHandle = _swapchain.handle();

	uint32_t imageIndex;
	vkAcquireNextImageKHR(_graphics.device(), swapChainHandle,
		std::numeric_limits<uint64_t>::max(), _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkCommandBuffer cmdBuffers[] = { _commandPool[imageIndex] };
	VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { _renderFinishedSemaphore };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = util::array_size<uint32_t>(waitSemaphores);
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = util::array_size<uint32_t>(cmdBuffers);
	submitInfo.pCommandBuffers = cmdBuffers;
	submitInfo.signalSemaphoreCount = util::array_size<uint32_t>(signalSemaphores);
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(_graphics.graphics_queue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		throw Exception("failed to submit queue");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &_renderFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChainHandle;
	presentInfo.pImageIndices = &imageIndex;

	auto result = vkQueuePresentKHR(_graphics.present_queue(), &presentInfo);

	if ((result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR))
		_recreate_swapchain();
}

void redox::RenderSystem::wait_pending() {
	vkDeviceWaitIdle(_graphics.device());
}

void redox::RenderSystem::_recreate_swapchain() {
	wait_pending();

	RDX_LOG("Recreating swapchain...");

	util::reconstruct(_swapchain, _graphics);
	util::reconstruct(_pipeline, _graphics, _swapchain);
	util::reconstruct(_commandPool, _graphics, _swapchain.size());

	//TODO: DEMO
	demo_setup();
}

void redox::RenderSystem::_init_semaphores() {
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(_graphics.device(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(_graphics.device(), &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS) {

		throw Exception("failed to create semaphores");
	}
}
