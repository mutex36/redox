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
#include "model_factory.h"

#include "resources/importer/gltf_importer.h"
#include "graphics/vulkan/graphics.h"
#include "core/application.h"

redox::graphics::ModelFactory::ModelFactory(const DescriptorPool* dp, PipelineCache* pc) 
: _descriptorPool(dp), _pipelineCache(pc) {
}

redox::ResourceHandle<redox::IResource> redox::graphics::ModelFactory::load(const Path& path) {
	GLTFImporter importer(path);

	//import meshes
	redox::Buffer<ResourceHandle<Mesh>> meshes;
	meshes.reserve(importer.mesh_count());

	for (std::size_t i = 0; i < importer.mesh_count(); i++) {
		auto mesh = importer.import_mesh(i);

		redox::Buffer<MeshVertex> vertices;
		vertices.reserve(mesh.vertexCount);

		for (std::size_t i = 0; i < vertices.capacity(); ++i) {
			vertices.push_back({
				{ mesh.positions[i * 3 + 0], mesh.positions[i * 3 + 1], mesh.positions[i * 3 + 2] },
				( mesh.normals.empty() ? math::Vec3f{} : math::Vec3f{ mesh.normals[i * 3 + 0], mesh.normals[i * 3 + 1], mesh.normals[i * 3 + 2] }),
				( mesh.texcoords.empty() ? math::Vec2f{} : math::Vec2f{ mesh.texcoords[i * 2 + 0], mesh.texcoords[i * 2 + 1] })
			});
		}

		redox::Buffer<SubMesh> submeshes;
		submeshes.reserve(mesh.submeshes.size());

		for (auto& sm : mesh.submeshes) {
			SubMesh submesh{};
			submesh.materialIndex = static_cast<uint32_t>(sm.materialIndex);
			submesh.indexCount = static_cast<uint32_t>(sm.indexCount);
			submesh.indexOffset = static_cast<uint32_t>(sm.indexOffset);

			submeshes.push_back(submesh);
		}

		meshes.push_back(std::make_shared<Mesh>(
			std::move(vertices), std::move(mesh.indices), std::move(submeshes)));
	}

	//import materials
	redox::Buffer<ResourceHandle<Material>> materials;
	materials.reserve(importer.material_count());

	auto resources = ResourceManager::instance();

	for (std::size_t i = 0; i < importer.material_count(); i++) {
		auto impMat = importer.import_material(i);
		auto pipeline = _pipelineCache->load(PipelineType::DEFAULT_MESH_PIPELINE);
		auto dset = _descriptorPool->allocate(pipeline->descriptorLayout());

		auto& material = materials.emplace_back(std::make_shared<Material>(pipeline, dset));

		Path albedoPath(impMat.albedoMap);
		auto albedo = resources->load<SampleTexture>(
			"textures" / albedoPath.filename(),
			"builtin:textures/uvcheck.png"
		);
		material->set_texture(TextureKeys::ALBEDO, std::move(albedo));

		Path normalPath(impMat.normalMap);
		auto normal = resources->load<SampleTexture>(
			"textures" / normalPath.filename(),
			"builtin:textures/uvcheck.png"
		);
		material->set_texture(TextureKeys::NORMAL, std::move(normal));
	}

	return std::make_shared<Model>(std::move(meshes), std::move(materials));
}

bool redox::graphics::ModelFactory::supports_ext(const Path& ext) {
	return (ext == ".gltf");
}

