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
#include "staged_buffer.h"
#include "math\math.h"

namespace redox::graphics {
	class Graphics;
	class CommandBuffer;

	struct MeshVertex {
		math::Vec2f pos;
		math::Vec3f color;
	};

	class Mesh {
	public:
		Mesh(const redox::Buffer<MeshVertex>& vertices, 
			const redox::Buffer<uint16_t>& indices, const Graphics& graphics);
		~Mesh() = default;

		void bind(const CommandBuffer& commandBuffer);
		void upload(const CommandBuffer& commandBuffer);

		uint32_t vertex_count() const;
		uint32_t instance_count() const;

	private:
		uint32_t _vertexCount;
		uint32_t _instanceCount;
		const Graphics& _graphicsRef;
		
		StagedBuffer _indexBuffer;
		StagedBuffer _vertexBuffer;
	};
}