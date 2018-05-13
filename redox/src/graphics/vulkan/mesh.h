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
#include "buffer_base.h"
#include "vertex_layout.h"
#include "resources\factory.h"

#include "platform\filesystem.h"

namespace redox::graphics {
	class Graphics;

	class Mesh {
	public:
		template<class Vertex>
		Mesh(const Buffer<Vertex>& vertices, const Buffer<uint16_t>& indices, const Graphics& graphics, const CommandPool& commandPool)
			: _graphicsRef(graphics), _vertexCount(vertices.size()),
			_vertexBuffer(vertices.byte_size(), graphics, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
			_indexBuffer(indices.byte_size(), graphics, VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {

			_vertexBuffer.map([&vertices](void* dest) {
				std::memcpy(dest, vertices.data(), vertices.byte_size());
			});

			_indexBuffer.map([&indices](void* dest) {
				std::memcpy(dest, indices.data(), indices.byte_size());
			});

			_vertexBuffer.transfer(commandPool);
			_indexBuffer.transfer(commandPool);
		}
		~Mesh() = default;

		uint32_t vertex_count() const;
		const BufferBase& vertex_buffer() const;
		const BufferBase& index_buffer() const;

	private:
		uint32_t _vertexCount;
		const Graphics& _graphicsRef;
		
		BufferBase _indexBuffer;
		BufferBase _vertexBuffer;
	};
}