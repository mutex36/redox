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
#include "mesh.h"

#include <array> //std::array

namespace redox::graphics {

	template<class T>
	struct layout_traits;

	template<>
	struct layout_traits<MeshVertex> {
		constexpr VkVertexInputBindingDescription binding_desc() {
			return { 0, sizeof(MeshVertex), VK_VERTEX_INPUT_RATE_VERTEX };
		}

		constexpr std::array<VkVertexInputAttributeDescription, 2> atr_desc() {
			return { {
				{ 0, 0, VK_FORMAT_R32G32_SFLOAT, util::offset_of<uint32_t>(&MeshVertex::pos) },
				{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, util::offset_of<uint32_t>(&MeshVertex::color) }
			} };
		}
	};

}