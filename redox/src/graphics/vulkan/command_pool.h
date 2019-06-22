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
#include "commands.h"
#include "resources/mesh.h"
#include "resources/material.h"

namespace redox::graphics {

	class CommandBufferView {
	public:
		CommandBufferView(VkCommandBuffer handle);
		~CommandBufferView() = default;

		void submit(UniquePtr<ICommand> command);

		[[nodiscard]] auto scoped_record() {
			begin_record();
			return make_scope_guard([this]() { end_record(); });
		}
		void begin_record();
		void end_record();

		VkCommandBuffer handle() const;

	private:
		//void _flush();
		//redox::Buffer<UniquePtr<ICommand>> _commands;
		VkCommandBuffer _handle;
	};

	class CommandPool : public NonCopyable {
	public:
		CommandPool(VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		~CommandPool();

		void free_all();
		void allocate(uint32_t numBuffers);

		CommandBufferView operator[](std::size_t index) const;

	protected:
		VkCommandPool _handle;
		redox::Buffer<VkCommandBuffer> _commandBuffers;
	};

	class AuxCommandPool : public CommandPool {
	public:
		AuxCommandPool();
		~AuxCommandPool();

		static const AuxCommandPool& instance() {
			static AuxCommandPool instance;
			return instance;
		}

		void submit(FunctionRef<void(
			const CommandBufferView&)> fn, bool sync = true,
			uint64_t timeout = std::numeric_limits<uint64_t>::max()) const noexcept;

	private:
		VkFence _queueFence;
	};

}