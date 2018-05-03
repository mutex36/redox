#pragma once
#include "vulkan.h"

#include "math\math.h"
#include "core\utility.h"

namespace redox {
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
			layout.attrDesc[0].offset = util::offset_of(&DefaultVertex::pos);
			layout.attrDesc[1].binding = 0;
			layout.attrDesc[1].location = 1;
			layout.attrDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			layout.attrDesc[1].offset = util::offset_of(&DefaultVertex::color);
			return layout;
		}

	};
}