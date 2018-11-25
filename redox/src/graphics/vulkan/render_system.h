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
#include "core\non_copyable.h"

#include "platform\window.h"
#include "graphics.h"
#include "render_pass.h"
#include "math\math.h"

namespace redox::graphics {

	class RenderSystem : public NonCopyable {
	public:
		static const RenderSystem* instance();

		RenderSystem(const platform::Window& window);
		~RenderSystem();

		void render();

	private:
		struct mvp_uniform {
			math::Mat44f model;
			math::Mat44f view;
			math::Mat44f projection;
		};
		
		void _swapchain_event_resize();
		void _pipeline_event_create(const PipelineHandle& pipeline);

		UniformBuffer _mvpBuffer;
		DescriptorPool _descriptorPool;

		//@DEMO
		ResourceHandle<Model> _demoModel;
		void _demo_draw();
		void _demo_load_assets();
		//@@@

		UniquePtr<Swapchain> _swapchain;
		UniquePtr<RenderPass> _forwardPass;
		UniquePtr<PipelineCache> _pipelineCache;

		UniquePtr<ModelFactory> _modelFactory;
		UniquePtr<TextureFactory> _textureFactory;
		UniquePtr<ShaderFactory> _shaderFactory;
	};
}