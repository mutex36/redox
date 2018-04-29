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
#pragma once
#include "core\core.h"
#include "core\buffer.h"

#include "graphics.h"
#include "vulkan.h"

namespace redox {

	class Swapchain {
	public:
		Swapchain(const Graphics& graphics);
		~Swapchain();

		VkSwapchainKHR handle() const;
		VkExtent2D extent() const;
		VkImageView operator[](std::size_t index) const;
		std::size_t size() const;

	private:
		void _init();
		void _init_images();

		void _destroy();

		Buffer<VkImage> _images;
		Buffer<VkImageView> _imageViews;

		VkSwapchainKHR _handle;
		VkExtent2D _extent;

		const Graphics& _graphicsRef;

	};
}