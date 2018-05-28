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
#include "pipeline.h"
#include "command_buffer.h"
#include "command_pool.h"

redox::graphics::Pipeline::Pipeline(const Graphics& graphics, const RenderPass& renderPass,
	const VertexLayout& vLayout, Resource<Shader> vs, Resource<Shader> fs) :
	_vs(std::move(vs)),
	_fs(std::move(fs)),
	_graphicsRef(graphics),
	_renderPassRef(renderPass) {

	_init_desriptors();
	_init(vLayout);
}

redox::graphics::Pipeline::~Pipeline() {
	vkDestroyDescriptorSetLayout(_graphicsRef.device(), _descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(_graphicsRef.device(), _descriptorPool, nullptr);
	vkDestroyPipeline(_graphicsRef.device(), _handle, nullptr);
	vkDestroyPipelineLayout(_graphicsRef.device(), _layout, nullptr);
}

void redox::graphics::Pipeline::bind(const CommandBuffer& commandBuffer, const Framebuffer& frameBuffer) {
	_renderPassRef.begin(frameBuffer, commandBuffer);

	vkCmdBindPipeline(commandBuffer.handle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _handle);
	vkCmdBindDescriptorSets(commandBuffer.handle(), VK_PIPELINE_BIND_POINT_GRAPHICS,
		_layout, 0, 1, &_descriptorSet, 0, nullptr);

	_update_viewport(commandBuffer);
}

void redox::graphics::Pipeline::unbind(const CommandBuffer& commandBuffer) {
	_renderPassRef.end(commandBuffer);
}

void redox::graphics::Pipeline::set_viewport(const VkExtent2D& size) {
	_viewport = size;
}

void redox::graphics::Pipeline::bind_resource(const Texture& texture, uint32_t bindingPoint) {

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture.view();
	imageInfo.sampler = texture.sampler();

	VkWriteDescriptorSet writeSet{};
	writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSet.dstSet = _descriptorSet;
	writeSet.dstBinding = bindingPoint;
	writeSet.dstArrayElement = 0;
	writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeSet.descriptorCount = 1;
	writeSet.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(_graphicsRef.device(), 1, &writeSet, 0, nullptr);
}

void redox::graphics::Pipeline::bind_resource(const UniformBuffer& ubo, uint32_t bindingPoint) {

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = ubo.handle();
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet writeSet{};
	writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSet.dstSet = _descriptorSet;
	writeSet.dstBinding = bindingPoint;
	writeSet.dstArrayElement = 0;
	writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeSet.descriptorCount = 1;
	writeSet.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(_graphicsRef.device(), 1, &writeSet, 0, nullptr);
}

void redox::graphics::Pipeline::_init(const VertexLayout& vLayout) {

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vLayout.attribs.size());
	vertexInputInfo.pVertexAttributeDescriptions = vLayout.attribs.data();
	vertexInputInfo.pVertexBindingDescriptions = &vLayout.binding;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampleState{};
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = nullptr;
	viewportState.scissorCount = 1;
	viewportState.pScissors = nullptr;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;

	if (vkCreatePipelineLayout(_graphicsRef.device(), &pipelineLayoutInfo, nullptr, &_layout) != VK_SUCCESS)
		throw Exception("failed to create pipeline layout");

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = _vs->handle();
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = _fs->handle();
	fragShaderStageInfo.pName = "main";

	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount = util::array_size<uint32_t>(dynamicStates);
	dynamicStateInfo.pDynamicStates = dynamicStates;

	const VkPipelineShaderStageCreateInfo shaderStages[] = 
		{ vertShaderStageInfo, fragShaderStageInfo };

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = util::array_size<uint32_t>(shaderStages);
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampleState;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.layout = _layout;
	pipelineInfo.renderPass = _renderPassRef.handle();
	pipelineInfo.subpass = 0;

	if (vkCreateGraphicsPipelines(
		_graphicsRef.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_handle) != VK_SUCCESS)
			throw Exception("failed to create pipeline");
}

void redox::graphics::Pipeline::_init_desriptors() {

	//TODO: variable binding layout
	redox::Buffer<VkDescriptorSetLayoutBinding> bindings(2);

	bindings[0].binding = 0;
	bindings[0].descriptorCount = 1;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	bindings[1].binding = 1;
	bindings[1].descriptorCount = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(_graphicsRef.device(), &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS)
		throw Exception("failed to create descriptor set layout");

	VkDescriptorPoolSize poolSizes[] = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
	};

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = util::array_size<uint32_t>(poolSizes);
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(_graphicsRef.device(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
		throw Exception("failed to create descriptor pool");

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &_descriptorSetLayout;

	if (vkAllocateDescriptorSets(_graphicsRef.device(), &allocInfo, &_descriptorSet) != VK_SUCCESS)
		throw Exception("failed to allocate descriptor set");
}

void redox::graphics::Pipeline::_update_viewport(const CommandBuffer& commandBuffer) {
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(_viewport.width);
	viewport.height = static_cast<float>(_viewport.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer.handle(), 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = _viewport;
	vkCmdSetScissor(commandBuffer.handle(), 0, 1, &scissor);
}
