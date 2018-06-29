#pragma once
#include "core/hashmap.h"
#include "core/ref_counted.h"

#include "pipeline.h"

namespace redox::graphics {
	class Graphics;
	class RenderPass;
	class ShaderFactory;

	enum class PipelineType {
		DEFAULT_MESH_PIPELINE,
		SKINNED_MESH_PIPELINE,
		DEFAULT_2D_PIPELINE,
		TERRAIN_PIPELINE,
		INVALID
	};

	using PipelineHandle = RefCounted<Pipeline>;

	class PipelineCache {
	public:
		PipelineCache(const Graphics& graphics, 
			const RenderPass& renderPass, const ShaderFactory& shaderFactory);

		PipelineHandle load(PipelineType type) const;

	private:
		PipelineHandle _create_pipeline(PipelineType type) const;
		PipelineHandle _create_default_mesh_pipeline() const;

		mutable Hashmap<PipelineType, PipelineHandle> _pipelines;

		const Graphics& _graphicsRef;
		const RenderPass& _renderPassRef;
		const ShaderFactory& _shaderFactoryRef;
	};
}