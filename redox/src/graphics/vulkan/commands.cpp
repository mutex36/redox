#include "commands.h"
#include "command_pool.h"

redox::graphics::IndexedDraw::IndexedDraw(ResourceHandle<Mesh> mesh, ResourceHandle<Material> material, const IndexRange& range) :
	_mesh(std::move(mesh)), _material(std::move(material)), _range(range) {
}

void redox::graphics::IndexedDraw::execute(const CommandBufferView & cb) {
	_material->bind(cb.handle());
	_mesh->bind(cb.handle());
	vkCmdDrawIndexed(cb.handle(), _range.count, 1, _range.start, 0, 0);
}

std::size_t redox::graphics::IndexedDraw::sort_key() const {
	return (std::size_t)_material.get();
}
