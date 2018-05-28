#include "texture_factory.h"

#define STB_IMAGE_IMPLEMENTATION
#include <thirdparty/stbimage/stb_image.h>

redox::Resource<redox::graphics::Texture> redox::graphics::TextureFactory::load_impl(
	const String& path, const Graphics& graphics) {

	i32 width, height, chan;
	stbi_uc* pixels = stbi_load(path.cstr(),
		&width, &height, &chan, STBI_rgb_alpha);

	if (pixels == nullptr)
		throw Exception("failed to load texture");

	auto size = width * height * 4;
	redox::Buffer<byte> buffer(pixels, size);

	stbi_image_free(pixels);
	Texture::Dimension dimensions{
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height) };

	return { construct_tag{}, std::move(buffer),
		VK_FORMAT_R8G8B8A8_UNORM, dimensions, graphics };
}
