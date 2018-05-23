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

redox::graphics::Swapchain::Swapchain(const Graphics& graphics, const RenderPass& renderPass, CreateCallback&& createCallback) :
	_createCallback(std::move(createCallback)),
	_graphicsRef(graphics),
	_renderPassRef(renderPass),
	_commandPool(graphics) {
	_init();
	_init_semaphores();
	_init_images();
	_init_fb();
	_createCallback();
}

void redox::graphics::Swapchain::_destroy() {
	for (auto& iv : _imageViews)
		vkDestroyImageView(_graphicsRef.device(), iv, nullptr);

	vkDestroySwapchainKHR(_graphicsRef.device(), _handle, nullptr);
	vkDestroySemaphore(_graphicsRef.device(), _renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(_graphicsRef.device(), _imageAvailableSemaphore, nullptr);
}

redox::graphics::Swapchain::~Swapchain() {
	_destroy();
}

void redox::graphics::Swapchain::present() {
	vkQueueWaitIdle(_graphicsRef.present_queue());

	uint32_t imageIndex;
	vkAcquireNextImageKHR(_graphicsRef.device(), _handle,
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

	if (vkQueueSubmit(_graphicsRef.graphics_queue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		throw Exception("failed to submit queue");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &_renderFinishedSemaphore;
	presentInfo.swapchainCount = util::array_size<uint32_t>(swapchains);
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	auto result = vkQueuePresentKHR(_graphicsRef.present_queue(), &presentInfo);
	if ((result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR))
		_reload();
}

void redox::graphics::Swapchain::_reload() {
	_graphicsRef.wait_pending();

	_destroy();
	_init();
	_init_semaphores();
	_init_images();
	_init_fb();
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

	if (vkCreateSemaphore(_graphicsRef.device(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(_graphicsRef.device(), &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS) {

		throw Exception("failed to create semaphores");
	}
}

void redox::graphics::Swapchain::_init() {

	VkSurfaceCapabilitiesKHR scp;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		_graphicsRef.physical_device(), _graphicsRef.surface(), &scp);

	_surfaceFormat = _graphicsRef.pick_surface_format();
	_presentMode = _graphicsRef.pick_presentation_mode();

	_extent.width = std::clamp(scp.currentExtent.width,
		scp.minImageExtent.width, scp.maxImageExtent.width);
	_extent.height = std::clamp(scp.currentExtent.height,
		scp.minImageExtent.height, scp.maxImageExtent.height);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = _graphicsRef.surface();
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

	if (vkCreateSwapchainKHR(_graphicsRef.device(), &createInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create swapchain");
}

void redox::graphics::Swapchain::_init_images() {
	_RDX_PROFILE;

	uint32_t imageCount;
	vkGetSwapchainImagesKHR(_graphicsRef.device(), _handle, &imageCount, nullptr);
	_images.resize(imageCount);
	vkGetSwapchainImagesKHR(_graphicsRef.device(), _handle, &imageCount, _images.data());

	_imageViews.resize(imageCount);

	_commandPool.free_all();
	_commandPool.allocate(imageCount);

	for (std::size_t i = 0; i < _images.size(); ++i) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _images[i];
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

		if (vkCreateImageView(_graphicsRef.device(), &createInfo, nullptr, &_imageViews[i]) != VK_SUCCESS)
			throw Exception("failed to create image view");
	}
}

void redox::graphics::Swapchain::_init_fb() {
	_RDX_PROFILE;
	_frameBuffers.clear();
	_frameBuffers.reserve(_imageViews.size());

	for (size_t i = 0; i < _frameBuffers.capacity(); i++)
		_frameBuffers.emplace(_graphicsRef, _renderPassRef.handle(), _imageViews[i], _extent);
}