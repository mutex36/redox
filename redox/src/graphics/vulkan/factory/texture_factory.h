#pragma once
#include "resources\factory.h"
#include "graphics\vulkan\texture.h"

#include <thirdparty/stbimage/stb_image.h>

namespace redox::graphics {
	class Graphics;

	class TextureFactory : public ResourceFactory<TextureFactory, Texture> {
		friend class ResourceFactory<TextureFactory, Texture>;

		Resource<Texture> load_impl(const io::Path& path, const Graphics& graphics) {

			int width, height, chan;
			stbi_uc* pixels = stbi_load(path.cstr(), 
				&width, &height, &chan, STBI_rgb_alpha);

			if (pixels == nullptr)
				throw Exception("failed to load texture");

			auto size = width * height * 4;
			redox::Buffer<byte> buffer(pixels, size);

			stbi_image_free(pixels);
			Texture::Dimension dimensions{ width, height };

			return { construct_tag{}, std::move(buffer),
				VK_FORMAT_R8G8B8A8_UNORM, dimensions, graphics };
		}
	};
}