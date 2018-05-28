#pragma once
#include "resources\factory.h"
#include "graphics\vulkan\texture.h"

namespace redox::graphics {
	class Graphics;

	class TextureFactory : public ResourceFactory<TextureFactory, Texture> {
		friend class ResourceFactory<TextureFactory, Texture>;

		Resource<Texture> load_impl(const String& path, const Graphics& graphics);
	};
}