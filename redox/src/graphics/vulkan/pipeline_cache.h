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
#pragma once
#include "pipeline.h"

namespace redox::graphics {
	class RenderPass;

	enum class PipelineType {
		DEFAULT_MESH_PIPELINE,
		SKINNED_MESH_PIPELINE,
		DEFAULT_2D_PIPELINE,
		TERRAIN_PIPELINE
	};

	using PipelineHandle = SharedPtr<Pipeline>;

	class PipelineCache : public NonCopyable {
	public:
		using PipelineCreatedCallback = Function<void(const PipelineHandle&)>;

		PipelineCache(PipelineCreatedCallback&& pcc, const RenderPass* rp);

		PipelineHandle load(PipelineType type) const;

		auto begin() const { return _pipelines.begin(); }
		auto end() const { return _pipelines.end(); }

	private:
		PipelineHandle _create_pipeline(PipelineType type) const;
		PipelineHandle _create_default_mesh_pipeline() const;

		PipelineCreatedCallback _createCallback;
		const UniformBuffer* _mvpBuffer;
		const RenderPass* _renderPass;
		mutable Hashmap<PipelineType, PipelineHandle> _pipelines;
	};
}