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
#include "platform/filesystem.h"
#include "graphics/vulkan/resources/mesh.h"
#include "core/hashmap.h"
#include "resources/resource.h"

#include <thirdparty/gltf/cgltf.h>

namespace redox {
	class GLTFImporter : public NonCopyable {
	public:
		GLTFImporter(const String& path);
		~GLTFImporter();

		struct submesh_data {
			std::size_t attributeOffset;
			std::size_t attributeCount;
			std::size_t indexOffset;
			std::size_t indexCount;
			std::size_t materialIndex;
		};

		struct mesh_data {
			redox::String name;
			std::size_t vertexCount;
			Buffer<float_t> positions;
			Buffer<float_t> texcoords;
			Buffer<float_t> normals;
			Buffer<uint16_t> indices;
			Buffer<submesh_data> submeshes;
		};

		struct material_data {
			redox::String name;
			redox::String albedoMap;
			redox::String normalMap;
		};

		std::size_t mesh_count() const;
		std::size_t material_count() const;

		material_data import_material(std::size_t index);
		mesh_data import_mesh(std::size_t index);

	private:
		template<class ParseType, class Fn>
		void read_buffer(cgltf_buffer_view* bufferView, cgltf_accessor* accessor, Fn&& fn) {
			auto it = _buffers.get(bufferView->buffer->uri);
			if (it == _buffers.end()) {
				io::File blobFile(RDX_FIND_ASSET("meshes\\", bufferView->buffer->uri),
					io::File::Mode::READ);

				it = _buffers.push(bufferView->buffer->uri, blobFile.read());
			}

			auto readOffset = bufferView->offset + accessor->offset;
			auto readSize = accessor->count * accessor->stride;

			for (size_t bufferIndex = readOffset;
				bufferIndex < readOffset + readSize; bufferIndex += sizeof(ParseType)) {
				fn(reinterpret_cast<const ParseType&>(it->value[bufferIndex]));
			}
		}

		cgltf_data _data;
		Hashmap<String, Buffer<i8>> _buffers;
	};
}