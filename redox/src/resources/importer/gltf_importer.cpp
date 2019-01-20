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
#include "gltf_importer.h"
#include "core/logging/log.h"

redox::GLTFImporter::GLTFImporter(const Path& filePath) :
	_searchPath(filePath.parent_path()) {

	io::File file(filePath, io::File::Mode::READ | io::File::Mode::THROW_IF_INVALID);
	auto buffer = file.read();

	cgltf_options options{};
	cgltf_result result = cgltf_parse(&options, buffer.data(), buffer.size(), &_data);
	if (result != cgltf_result_success) {
		throw Exception("failed to load gltf file");
	}
}

redox::GLTFImporter::~GLTFImporter() {
	cgltf_free(&_data);
}

std::size_t redox::GLTFImporter::mesh_count() const {
	return _data.meshes_count;
}

std::size_t redox::GLTFImporter::material_count() const {
	return _data.material_count;
}

redox::GLTFImporter::material_data redox::GLTFImporter::import_material(std::size_t index) {
	if (index >= _data.material_count) {
		throw Exception("material index not found");
	}

	const auto& material = _data.materials[index];
	material_data output{ material.name ? material.name : "unknown" };

	if (material.pbr.base_color_texture.texture) {
		output.albedoMap = material.pbr.base_color_texture.texture->image->uri;
	}

	if (material.normal_texture.texture) {
		output.normalMap = material.normal_texture.texture->image->uri;
	}

	if (material.occlusion_texture.texture) {
		output.aoMap = material.occlusion_texture.texture->image->uri;
	}

	return output;
}

redox::GLTFImporter::mesh_data redox::GLTFImporter::import_mesh(std::size_t index) {

	if (index >= _data.meshes_count) {
		throw Exception("mesh index not found");
	}

	const auto& mesh = _data.meshes[index];
	mesh_data output{ mesh.name ? mesh.name : "" };

	output.submeshes.reserve(mesh.primitives_count);

	for (std::size_t primIndex = 0; primIndex < mesh.primitives_count; primIndex++) {
		const auto& primitive = mesh.primitives[primIndex];

		if (primitive.type != cgltf_type_triangles) {
			throw Exception("unsupported primitive type");
		}

		submesh_data submesh{};
		submesh.indexOffset = output.indices.size();
		output.indices.reserve(primitive.indices->count);
		read_buffer<uint16_t>(primitive.indices->buffer_view,
			primitive.indices, [&output](const auto& value) {
			output.indices.push_back(value);
		});
		submesh.indexCount = output.indices.size() - submesh.indexOffset;
		submesh.attributeOffset = output.positions.size() / 3;

		for (std::size_t attrIndex = 0; attrIndex < primitive.attributes_count; attrIndex++) {
			const auto& attribute = primitive.attributes[attrIndex];
			const auto& bufferView = attribute.data->buffer_view;

			if (bufferView->type != cgltf_buffer_view_type_vertices) {
				RDX_LOG("Skipped buffer of type: {0}", bufferView->type);
				continue;
			}

			switch (attribute.name) {
			case cgltf_attribute_type_position: {

				output.positions.reserve(output.positions.capacity() + attribute.data->count * 3);
				read_buffer<float_t>(bufferView, attribute.data, [&output](const auto& value) {
					output.positions.push_back(value);
				});

				break;
			}
			case cgltf_attribute_type_normal: {

				output.normals.reserve(output.normals.capacity() + attribute.data->count * 3);
				read_buffer<float_t>(bufferView, attribute.data, [&output](const auto& value) {
					output.normals.push_back(value);
				});

				break;
			}

			case cgltf_attribute_type_texcoord_0: {

				output.texcoords.reserve(output.texcoords.capacity() + attribute.data->count * 2);
				read_buffer<float_t>(bufferView, attribute.data, [&output](const auto& value) {
					output.texcoords.push_back(value);
				});

				break;
			}
			}
		}

		submesh.attributeCount = output.positions.size() / 3 - submesh.attributeOffset;
		submesh.materialIndex = primitive.material - _data.materials;
		output.submeshes.push_back(std::move(submesh));
	}

	output.vertexCount = output.positions.size() / 3;
	return output;
}