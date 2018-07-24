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
#include "pipeline_cache.h"

#include "graphics.h"
#include "core/application.h"

redox::graphics::PipelineCache::PipelineCache() :
	_pipelines(PipelineType::INVALID) {
}

redox::graphics::PipelineHandle redox::graphics::PipelineCache::load(PipelineType type) const {
	auto hit = _pipelines.get(type);

	if (hit != _pipelines.end())
		return hit->value;

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

	VkDescriptorSetLayoutBinding mvpBinding{};
	mvpBinding.binding = 0;
	mvpBinding.descriptorCount = 1;
	mvpBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mvpBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding albedoBinding{};
	albedoBinding.binding = 1;
	albedoBinding.descriptorCount = 1;
	albedoBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	albedoBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	DescriptorLayout dLayout{ { mvpBinding, albedoBinding } };

	VertexLayout vLayout{};
	vLayout.binding.binding = 0;
	vLayout.binding.stride = sizeof(MeshVertex);
	vLayout.binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	vLayout.attribs.resize(3);
	vLayout.attribs[0].binding = 0;
	vLayout.attribs[0].location = 0;
	vLayout.attribs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vLayout.attribs[0].offset = util::offset_of<uint32_t>(&MeshVertex::pos);

	vLayout.attribs[1].binding = 0;
	vLayout.attribs[1].location = 1;
	vLayout.attribs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vLayout.attribs[1].offset = util::offset_of<uint32_t>(&MeshVertex::normal);

	vLayout.attribs[2].binding = 0;
	vLayout.attribs[2].location = 2;
	vLayout.attribs[2].format = VK_FORMAT_R32G32_SFLOAT;
	vLayout.attribs[2].offset = util::offset_of<uint32_t>(&MeshVertex::uv);

	const auto& resources = Application::instance->resource_manager();
	
	auto vs = resources.load<Shader>("shader\\vert.spv");
	auto fs = resources.load<Shader>("shader\\frag.spv");

	auto pipeline = std::make_shared<Pipeline>(
		Graphics::instance->forward_render_pass(), vLayout, dLayout, std::move(vs), std::move(fs));

	return _pipelines.emplace(PipelineType::DEFAULT_MESH_PIPELINE, std::move(pipeline));
}
