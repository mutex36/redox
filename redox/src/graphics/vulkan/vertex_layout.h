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
#include "vulkan.h"

#include "math\math.h"
#include "core\utility.h"

namespace redox::graphics {
	struct VertexLayout {
		VkVertexInputBindingDescription bindingDesc;
		Buffer<VkVertexInputAttributeDescription> attrDesc;
	};

	struct DefaultVertex {
		math::Vec2f pos;
		math::Vec3f color;

		static VertexLayout get_layout() {
			VertexLayout layout{ {},{2} };
			layout.bindingDesc.binding = 0;
			layout.bindingDesc.stride = sizeof(DefaultVertex);
			layout.bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			layout.attrDesc[0].binding = 0;
			layout.attrDesc[0].location = 0;
			layout.attrDesc[0].format = VK_FORMAT_R32G32_SFLOAT;
			layout.attrDesc[0].offset = static_cast<uint32_t>(util::offset_of(&DefaultVertex::pos));
			layout.attrDesc[1].binding = 0;
			layout.attrDesc[1].location = 1;
			layout.attrDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			layout.attrDesc[1].offset = static_cast<uint32_t>(util::offset_of(&DefaultVertex::color));
			return layout;
		}

	};
}