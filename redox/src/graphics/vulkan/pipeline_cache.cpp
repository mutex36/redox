#include "pipeline_cache.h"

#include "graphics.h"
#include "render_pass.h"
#include "factory/shader_factory.h"

redox::graphics::PipelineCache::PipelineCache(const Graphics& graphics, const RenderPass& renderPass, const ShaderFactory& shaderFactory) :
	_pipelines(PipelineType::INVALID),
	_renderPassRef(renderPass),
	_graphicsRef(graphics),
	_shaderFactoryRef(shaderFactory) {
}

redox::graphics::PipelineHandle redox::graphics::PipelineCache::load(PipelineType type) const {
	auto hit = _pipelines.get(type);

	if (hit)
		return *hit;

	return _create_pipeline(type);
}

redox::graphics::PipelineHandle redox::graphics::PipelineCache::_create_pipeline(PipelineType type) const {

	switch (type) {
	case redox::graphics::PipelineType::DEFAULT_MESH_PIPELINE:
		return _create_default_mesh_pipeline();
	}
	
	throw Exception("invalid pipeline type");
}

redox::graphics::PipelineHandle redox::graphics::PipelineCache::_create_default_mesh_pipeline() const {
	redox::Buffer<VkDescriptorSetLayoutBinding> bindings(2);

	bindings[0].binding = 0;
	bindings[0].descriptorCount = 1;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	bindings[1].binding = 1;
	bindings[1].descriptorCount = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	auto vs = _shaderFactoryRef.load(RDX_ASSET("shader\\vert.spv"));
	auto fs = _shaderFactoryRef.load(RDX_ASSET("shader\\frag.spv"));
	auto vl = get_layout<MeshVertex>();

	return _pipelines.emplace(PipelineType::DEFAULT_MESH_PIPELINE, construct_tag{}, _graphicsRef, _renderPassRef,
		std::move(vl), std::move(bindings), std::move(vs), std::move(fs));
}
