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

namespace {
	bool load_image(const redox::Path& path, redox::Buffer<redox::byte>& buffer, redox::i32& width, redox::i32& height) {
		[[maybe_unused]] redox::i32 chan;
		auto ps = path.string();
		stbi_uc* pixels = stbi_load(ps.c_str(),
			&width, &height, &chan, STBI_rgb_alpha);

		if (pixels == nullptr) {
			RDX_LOG("failed to load image: {0}", redox::ConsoleColor::RED, stbi_failure_reason());
			return false;
		}

		RDX_SCOPE_GUARD([pixels]() {
			stbi_image_free(pixels);
		});

		auto size = width * height * 4;
		buffer.assign(pixels, pixels + size);
		return true;
	}
}

redox::ResourceHandle<redox::IResource> redox::graphics::TextureFactory::load(const Path& path) {
	i32 width, height;
	redox::Buffer<byte> buffer;
	if (!load_image(path, buffer, width, height)) {
		return nullptr;
	}

	return std::make_shared<SampleTexture>(
		std::move(buffer), VK_FORMAT_R8G8B8A8_UNORM,
		VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) 
	});
}

bool redox::graphics::TextureFactory::supports_ext(const Path& ext) {
	Array<StringView, 9> supported = { ".jpg", ".png", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic" };
	return std::find(supported.begin(), supported.end(), ext) != supported.end();
}

