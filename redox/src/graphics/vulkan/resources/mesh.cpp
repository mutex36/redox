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
#include "mesh.h"
#include "graphics\vulkan\graphics.h"
#include "graphics\vulkan\command_pool.h"

redox::graphics::Mesh::Mesh(const redox::Buffer<MeshVertex>& vertices,
	const redox::Buffer<uint16_t>& indices, redox::Buffer<SubMesh> submeshes) :
	_vertexCount(vertices.size()),
	_indexCount(indices.size()),
	_submeshes(std::move(submeshes)),
	_vertexBuffer(util::byte_size(vertices)),
	_indexBuffer(util::byte_size(indices)) {

	_vertexBuffer.map([&vertices](void* dest) {
		std::memcpy(dest, vertices.data(), util::byte_size(vertices));
	});

	_indexBuffer.map([&indices](void* dest) {
		std::memcpy(dest, indices.data(), util::byte_size(indices));
	});
}

void redox::graphics::Mesh::bind(const CommandBufferView& commandBuffer) {
	VkBuffer vb = _vertexBuffer.handle();
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer.handle(), 0, 1, &vb, &offset);
	vkCmdBindIndexBuffer(commandBuffer.handle(), _indexBuffer.handle(), 0, VK_INDEX_TYPE_UINT16);
}

void redox::graphics::Mesh::upload() {
	_vertexBuffer.upload();
	_indexBuffer.upload();
}

redox::ResourceGroup redox::graphics::Mesh::res_group() const {
	return ResourceGroup::GRAPHICS;
}

uint32_t redox::graphics::Mesh::vertex_count() const {
	return _vertexCount;
}

uint32_t redox::graphics::Mesh::index_count() const {
	return _indexCount;
}

const redox::Buffer<redox::graphics::SubMesh>& redox::graphics::Mesh::submeshes() const {
	return _submeshes;
}
