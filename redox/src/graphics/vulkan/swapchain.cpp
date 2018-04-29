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

redox::Swapchain::Swapchain(const Graphics& graphics) : _graphicsRef(graphics) {
	_init();
	_init_images();
}

redox::Swapchain::~Swapchain() {
	_destroy();
}

VkSwapchainKHR redox::Swapchain::handle() const {
	return _handle;
}

VkExtent2D redox::Swapchain::extent() const {
	return _extent;
}

VkImageView redox::Swapchain::operator[](std::size_t index) const {
	return _imageViews[index];
}

std::size_t redox::Swapchain::size() const {
	return _imageViews.size();
}

void redox::Swapchain::_init() {

	VkSurfaceCapabilitiesKHR scp;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		_graphicsRef.physical_device(), _graphicsRef.surface(), &scp);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		_graphicsRef.physical_device(), _graphicsRef.surface(), &formatCount, nullptr);

	Buffer<VkSurfaceFormatKHR> formats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		_graphicsRef.physical_device(), _graphicsRef.surface(), &formatCount, formats.data());


	//TODO: look at VK_PRESENT_MODE_MAILBOX_KHR
	_extent.width = std::clamp(scp.currentExtent.width,
		scp.minImageExtent.width, scp.maxImageExtent.width);
	_extent.height = std::clamp(scp.currentExtent.height,
		scp.minImageExtent.height, scp.maxImageExtent.height);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = _graphicsRef.surface();
	createInfo.minImageCount = scp.minImageCount;
	createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	createInfo.imageExtent = _extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = scp.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(_graphicsRef.device(), &createInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create swapchain");
}

void redox::Swapchain::_init_images() {

	uint32_t imageCount;
	vkGetSwapchainImagesKHR(_graphicsRef.device(), _handle, &imageCount, nullptr);
	_images.resize(imageCount);
	vkGetSwapchainImagesKHR(_graphicsRef.device(), _handle, &imageCount, _images.data());

	_imageViews.resize(imageCount);

	for (std::size_t i = 0; i < _images.size(); ++i) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
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

void redox::Swapchain::_destroy() {
	for (auto& iv : _imageViews)
		vkDestroyImageView(_graphicsRef.device(), iv, nullptr);
	vkDestroySwapchainKHR(_graphicsRef.device(), _handle, nullptr);
}
