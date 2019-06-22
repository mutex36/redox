#pragma once
#include "vulkan.h"
#include "resources/mesh.h"
#include "resources/material.h"
#include <resources/resource.h>

namespace redox::graphics {
	class CommandBufferView;

	struct IndexRange {
		uint32_t start, count;
	};

	class ICommand {
	public:
		virtual void execute(const CommandBufferView& cb) = 0;
		virtual std::size_t sort_key() const = 0;
	};

	class IndexedDraw : public ICommand {
	public:
		IndexedDraw(ResourceHandle<Mesh> mesh,
			ResourceHandle<Material> material, const IndexRange& range);
		void execute(const CommandBufferView& cb) override;
		std::size_t sort_key() const override;

	private:
		ResourceHandle<Mesh> _mesh;
		ResourceHandle<Material> _material;
		IndexRange _range;
	};
}