#include "mesh_factory.h"

#include "resources/importer/gltf_importer.h"

redox::Resource<redox::graphics::Mesh> redox::graphics::MeshFactory::load_impl(
	const String& path, const Graphics& graphics) {

	/*	const redox::Buffer<MeshVertex> vertices = {
	{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f } },
	{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 0.0f } },
	{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 1.0f } },
	{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f } }
	};
	const redox::Buffer<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
	};*/

	GLTFImporter importer(path);

	redox::Buffer<MeshVertex> vertices;
	redox::Buffer<uint16_t> indices;

	importer.import_mesh(0, vertices, indices);

	return { construct_tag{}, vertices, indices, graphics };
}