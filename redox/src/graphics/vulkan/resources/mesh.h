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
#include "graphics\vulkan\vulkan.h"
#include "graphics\vulkan\buffer.h"
#include "math\math.h"
#include "resources\resource.h"

namespace redox::graphics {
	class CommandBufferView;

	struct MeshVertex {
		math::Vec3f pos;
		math::Vec3f normal;
		math::Vec2f uv;
	};

	struct SubMesh {
		uint32_t vertexOffset;
		uint32_t vertexCount;
		std::size_t materialIndex;
	};

	class Mesh : public IResource {
	public:
		Mesh(const redox::Buffer<MeshVertex>& vertices, 
			const redox::Buffer<uint16_t>& indices, redox::Buffer<SubMesh> submeshes);
		~Mesh() override = default;

		void bind(const CommandBufferView& commandBuffer);
		void upload() override;

		uint32_t vertex_count() const;
		uint32_t index_count() const;

		const redox::Buffer<SubMesh>& submeshes() const;

	private:
		uint32_t _vertexCount;
		uint32_t _indexCount;

		redox::Buffer<SubMesh> _submeshes;

		IndexBuffer _indexBuffer;
		VertexBuffer _vertexBuffer;
	};
}