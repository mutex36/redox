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
#include "core\non_copyable.h"

#include "graphics.h"
#include "swapchain.h"
#include "pipeline.h"
#include "command_pool.h"
#include "render_pass.h"

#include "factory\mesh_factory.h"
#include "factory\shader_factory.h"
#include "factory\texture_factory.h"

#include "math\math.h"

namespace redox::graphics {
	class RenderSystem : public NonCopyable {
	public:
		RenderSystem(const platform::Window& window, const Configuration& config);
		~RenderSystem();

		void render();

		ShaderFactory& shader_factory();
		MeshFactory& mesh_factory();

	private:
		void _swapchain_event_create();

		struct mvp_uniform {
			math::Mat44f model;
			math::Mat44f view;
			math::Mat44f projection;
		};

		Graphics _graphics;
		CommandPool _auxCommandPool;

		UniformBuffer _mvpBuffer;
		MeshFactory _meshFactory;
		ShaderFactory _shaderFactory;
		TextureFactory _textureFactory;

		//@DEMO
		void _demo_setup();
		Resource<Mesh> _demoMesh;
		Resource<Texture> _demoTexture;
		Resource<Shader> _demoVs;
		Resource<Shader> _demoFs;

		RenderPass _renderPass;
		Pipeline _pipeline;
		Swapchain _swapchain;

		const Configuration& _configRef;
	};
}