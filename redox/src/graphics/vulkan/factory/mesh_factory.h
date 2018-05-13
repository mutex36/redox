#pragma once
#include "resources\factory.h"
#include "graphics\vulkan\mesh.h"

namespace redox::graphics {
	struct MeshFactory : public ResourceFactory<MeshFactory, Mesh> {
		template<class...Args>
		Resource<Mesh> internal_load(const io::Path& path, Args&&...args) {

			const Buffer<DefaultVertex> vertices = {
				{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
				{ { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
				{ { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } },
				{ { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f } }
			};
			const Buffer<uint16_t> indices = {
				0, 1, 2, 2, 3, 0
			};

			return make_resource<Mesh>(vertices, indices, std::forward<Args>(args)...);
		}
	};
}