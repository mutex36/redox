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
#include "swapchain.h"

#include "core\profiling\profiler.h"
#include <limits> //std::numeric_limits

#include "graphics.h"

redox::graphics::Swapchain::Swapchain(CreateCallback&& createCallback) :
	_createCallback(std::move(createCallback)) {
	_init();
	_init_semaphores();
	_init_images();
	_createCallback();
}

void redox::graphics::Swapchain::_destroy() {
	for (auto& iv : _imageViews)
		vkDestroyImageView(Graphics::instance().device(), iv, nullptr);

	vkDestroySwapchainKHR(Graphics::instance().device(), _handle, nullptr);
	vkDestroySemaphore(Graphics::instance().device(), _renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(Graphics::instance().device(), _imageAvailableSemaphore, nullptr);
}

redox::graphics::Swapchain::~Swapchain() {
	_destroy();
}

void redox::graphics::Swapchain::create_fbs(const RenderPass& renderPass) {
	_RDX_PROFILE;
	_frameBuffers.clear();
	_frameBuffers.reserve(_imageViews.size());

	for (size_t i = 0; i < _frameBuffers.capacity(); i++)
		_frameBuffers.emplace_back(renderPass, _imageViews[i], _extent);
}

void redox::graphics::Swapchain::visit(tl::function_ref<void(const Framebuffer&, const CommandBufferView&)> fn) const {
	for (std::size_t index = 0; index < _frameBuffers.size(); ++index)
		fn(_frameBuffers[index], _commandPool[index]);
}

void redox::graphics::Swapchain::present() {
	vkQueueWaitIdle(Graphics::instance().present_queue());

	uint32_t imageIndex;
	vkAcquireNextImageKHR(Graphics::instance().device(), _handle,
		std::numeric_limits<uint64_t>::max(), _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkCommandBuffer cmdBuffers[] = { _commandPool[imageIndex].handle() };
	VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { _renderFinishedSemaphore };
	VkSwapchainKHR swapchains[] = { _handle };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = util::array_size<uint32_t>(waitSemaphores);
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = util::array_size<uint32_t>(cmdBuffers);
	submitInfo.pCommandBuffers = cmdBuffers;
	submitInfo.signalSemaphoreCount = util::array_size<uint32_t>(signalSemaphores);
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(Graphics::instance().graphics_queue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		throw Exception("failed to submit queue");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &_renderFinishedSemaphore;
	presentInfo.swapchainCount = util::array_size<uint32_t>(swapchains);
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	auto result = vkQueuePresentKHR(Graphics::instance().present_queue(), &presentInfo);
	if ((result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR))
		_reload();
}

void redox::graphics::Swapchain::_reload() {
	Graphics::instance().wait_pending();

	_destroy();
	_init();
	_init_semaphores();
	_init_images();
	_createCallback();
}

VkSwapchainKHR redox::graphics::Swapchain::handle() const {
	return _handle;
}

VkExtent2D redox::graphics::Swapchain::extent() const {
	return _extent;
}

void redox::graphics::Swapchain::_init_semaphores() {
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(Graphics::instance().device(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(Graphics::instance().device(), &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS) {

		throw Exception("failed to create semaphores");
	}
}

void redox::graphics::Swapchain::_init() {

	VkSurfaceCapabilitiesKHR scp;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		Graphics::instance().physical_device(), Graphics::instance().surface(), &scp);

	_surfaceFormat = Graphics::instance().pick_surface_format();
	_presentMode = Graphics::instance().pick_presentation_mode();
	_extent.width = std::clamp(scp.currentExtent.width,
		scp.minImageExtent.width, scp.maxImageExtent.width);
	_extent.height = std::clamp(scp.currentExtent.height,
		scp.minImageExtent.height, scp.maxImageExtent.height);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = Graphics::instance().surface();
	createInfo.minImageCount = scp.minImageCount;
	createInfo.imageFormat = _surfaceFormat.format;
	createInfo.imageColorSpace = _surfaceFormat.colorSpace;
	createInfo.imageExtent = _extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = scp.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = _presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(Graphics::instance().device(), &createInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create swapchain");
}

void redox::graphics::Swapchain::_init_images() {
	_RDX_PROFILE;

	uint32_t imageCount;
	vkGetSwapchainImagesKHR(Graphics::instance().device(), _handle, &imageCount, nullptr);

	redox::Buffer<VkImage> images(imageCount);
	vkGetSwapchainImagesKHR(Graphics::instance().device(), _handle, &imageCount, images.data());

	_imageViews.resize(imageCount);
	_commandPool.free_all();
	_commandPool.allocate(imageCount);

	for (std::size_t i = 0; i < images.size(); ++i) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = _surfaceFormat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(Graphics::instance().device(), &createInfo, nullptr, &_imageViews[i]) != VK_SUCCESS)
			throw Exception("failed to create image view");
	}
}