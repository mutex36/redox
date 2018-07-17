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
#include "texture_factory.h"

#include "graphics/vulkan/pipeline_cache.h"
#include "resources/importer/gltf_importer.h"
#include "core/profiling/profiler.h"

redox::graphics::ModelFactory::ModelFactory(const PipelineCache& pipelineCache,
	const UniformBuffer& ubo, const TextureFactory& textureFactory, const DescriptorPool& dscPool) :
_pipelineCacheRef(pipelineCache),
_uboRef(ubo),
_textureFactoryRef(textureFactory),
_descPoolRef(dscPool) {
}

redox::ResourceHandle<redox::graphics::Model> redox::graphics::ModelFactory::load_impl(const String& path) const {
	_RDX_PROFILE;

	GLTFImporter importer(path);

	//import meshes
	redox::Buffer<ResourceHandle<Mesh>> meshes;
	meshes.reserve(importer.mesh_count());

	for (std::size_t i = 0; i < importer.mesh_count(); i++) {
		auto mesh = importer.import_mesh(i);

		redox::Buffer<MeshVertex> vertices;
		vertices.reserve(mesh.vertexCount);

		for (std::size_t i = 0; i < vertices.capacity(); ++i) {
			vertices.push({
				{ mesh.positions[i * 3 + 0], mesh.positions[i * 3 + 1], mesh.positions[i * 3 + 2] },
				{ mesh.normals[i * 3 + 0], mesh.normals[i * 3 + 1], mesh.normals[i * 3 + 2] },
				{ mesh.texcoords[i * 2 + 0], mesh.texcoords[i * 2 + 1] }
				});
		}

		redox::Buffer<SubMesh> submeshes;
		submeshes.reserve(mesh.submeshes.size());

		for (auto& sm : mesh.submeshes) {
			SubMesh submesh;
			submesh.materialIndex = static_cast<uint32_t>(sm.materialIndex);
			submesh.vertexCount = static_cast<uint32_t>(sm.indexCount);
			submesh.vertexOffset = static_cast<uint32_t>(sm.indexOffset);

			submeshes.push(submesh);
		}

		meshes.emplace(construct_tag{}, std::move(vertices),
			std::move(mesh.indices), std::move(submeshes));
	}

	//import materials
	redox::Buffer<ResourceHandle<Material>> materials;
	materials.reserve(importer.material_count());

	for (std::size_t i = 0; i < importer.material_count(); i++) {
		auto impMat = importer.import_material(i);

		auto albedoTexture = _textureFactoryRef.load(RDX_FIND_ASSET("textures\\", impMat.albedoMap));
		auto pipeline = _pipelineCacheRef.load(PipelineType::DEFAULT_MESH_PIPELINE);
		auto dset = _descPoolRef.allocate(pipeline->descriptorLayout());

		auto& material = materials.emplace(construct_tag{}, pipeline, dset);

		material->set_texture(TextureKeys::ALBEDO, std::move(albedoTexture));
		material->set_buffer(BufferKeys::MVP, _uboRef);
	}
	
	return { construct_tag{}, std::move(meshes), std::move(materials) };
}
