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
#include "vulkan.h"
#include "core\core.h"
#include "core\buffer.h"
#include "core\non_copyable.h"

#include "platform\window.h"

#include <type_traits> //std::forward

#include "graphics.h"
#include "swapchain.h"
#include "pipeline.h"
#include "command_pool.h"
#include "resources\factory.h"

namespace redox::graphics {
	class RenderSystem {
	public:
		RenderSystem(const platform::Window& window, const Configuration& config);
		~RenderSystem();

		void demo_setup();
		void render();

		const CommandPool& command_pool() const;
		const CommandPool& aux_command_pool() const;

	private:
		Resource<Mesh> _demoMesh;

		void _recreate_swapchain();
		void _init_semaphores();
		void _wait_pending();

		Graphics _graphics;
		Swapchain _swapchain;
		Pipeline _pipeline;
		CommandPool _commandPool;
		CommandPool _auxCommandPool;

		const Configuration& _configRef;

		VkSemaphore _imageAvailableSemaphore{ nullptr };
		VkSemaphore _renderFinishedSemaphore{ nullptr };
	};
}