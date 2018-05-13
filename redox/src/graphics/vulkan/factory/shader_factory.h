#pragma once
#include "resources\factory.h"
#include "graphics\vulkan\shader.h"

namespace redox::graphics {
	struct ShaderFactory : public ResourceFactory<ShaderFactory, Shader> {
		template<class...Args>
		Resource<Shader> internal_load(const io::Path& path, Args&&...args) {

			io::File fstream(path, io::File::Mode::READ);
			auto buffer = fstream.read();

			return make_resource<Shader>(std::move(buffer), std::forward<Args>(args)...);
		}
	};
}