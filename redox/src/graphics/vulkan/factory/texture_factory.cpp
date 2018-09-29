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
#include "texture_factory.h"

#define STB_IMAGE_IMPLEMENTATION
#include <thirdparty/stbimage/stb_image.h>

redox::ResourceHandle<redox::IResource> redox::graphics::TextureFactory::load(const String& path) {
	
	i32 chan, width, height;
	stbi_uc* pixels = stbi_load(path.cstr(),
		&width, &height, &chan, STBI_rgb_alpha);

	if (pixels == nullptr)
		throw Exception("failed to load texture");

	auto size = width * height * 4;
	redox::Buffer<byte> buffer(pixels, size);

	stbi_image_free(pixels);

	VkExtent2D dimensions{ 
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height) 
	};

	return std::make_shared<SampleTexture>(
		std::move(buffer), VK_FORMAT_R8G8B8A8_UNORM, dimensions);
}

bool redox::graphics::TextureFactory::supports_ext(const String& ext) {
	const StringView supportedExts[] = { ".jpg", ".png", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic" };
	for (const auto& sxt : supportedExts)
		if (sxt == ext)
			return true;

	return false;
}
