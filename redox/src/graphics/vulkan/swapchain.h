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
#include "command_pool.h"
#include "vulkan.h"
#include "framebuffer.h"

#include <functional> //std::function

namespace redox::graphics {
	class Swapchain {
	public:
		using RecreateCallback = std::function<void()>;

		Swapchain(const Graphics& graphics, RecreateCallback&& recreateCallback);
		~Swapchain();

		template<class Fn>
		void visit(Fn&& fn) {
			for (std::size_t index = 0; index < _frameBuffers.size(); ++index)
				fn(_frameBuffers[index], _commandPool[index]);
		}

		void present();

		VkSwapchainKHR handle() const;
		VkExtent2D extent() const;

	private:
		void _init();
		void _init_semaphores();
		void _init_images();
		void _init_fb();
		void _destroy();
		void _reload();

		RecreateCallback _recreateCallback;

		Buffer<VkImage> _images;
		Buffer<VkImageView> _imageViews;
		Buffer<Framebuffer> _frameBuffers;

		CommandPool _commandPool;

		VkSwapchainKHR _handle;
		VkExtent2D _extent;
		VkSurfaceFormatKHR _surfaceFormat;

		VkSemaphore _imageAvailableSemaphore;
		VkSemaphore _renderFinishedSemaphore;

		const Graphics& _graphicsRef;
	};
}