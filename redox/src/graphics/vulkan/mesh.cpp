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

#include "platform\filesystem.h"

redox::Mesh::Mesh(const io::Path& file, Graphics& graphics) : _graphicsRef(graphics) {


	auto size = static_cast<VkDeviceSize>();
	_init(size);

	void* data;
	vkMapMemory(_graphicsRef.device(), _memory, 0, size, 0, &data);
	memcpy(data, buffer.data(), size);
	vkUnmapMemory(_graphicsRef.device(), _memory);
}

redox::Mesh::~Mesh() {
	vkDestroyBuffer(_graphicsRef.device(), _handle, nullptr);
	vkFreeMemory(_graphicsRef.device(), _memory, nullptr);
}

void redox::Mesh::_init(VkDeviceSize bufferSize) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(_graphicsRef.device(), &bufferInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create vertex buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_graphicsRef.device(), _handle, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = _graphicsRef.find_memory_type(memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(_graphicsRef.device(), &allocInfo, nullptr, &_memory) != VK_SUCCESS)
		throw Exception("failed to allocate vertex buffer memory");

	vkBindBufferMemory(_graphicsRef.device(), _handle, _memory, 0);
}
