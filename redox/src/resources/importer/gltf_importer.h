#pragma once
#include "platform/filesystem.h"
#include "graphics/vulkan/mesh.h"
#include "core/hashmap.h"

#include <thirdparty/gltf/cgltf.h>

namespace redox {
	class GLTFImporter {
	public:
		GLTFImporter(const String& path);
		
		std::size_t mesh_count() const;
		void import_mesh(std::size_t index, 
			Buffer<graphics::MeshVertex>& vertices, Buffer<uint16_t>& indices);

	private:
		cgltf_data _data;
		Hashmap<redox::String, Buffer<i8>> _buffers;
	};
}