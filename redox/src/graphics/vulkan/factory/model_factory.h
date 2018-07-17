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
#include "resources\factory.h"
#include "graphics\vulkan\resources\model.h"

namespace redox::graphics {

	class TextureFactory;
	class PipelineCache;
	class DescriptorPool;

	class ModelFactory : public ResourceFactory<ModelFactory, Model> {
		friend class ResourceFactory<ModelFactory, Model>;

	public:
		ModelFactory(const PipelineCache& pipelineCache,
			const UniformBuffer& ubo, const TextureFactory& textureFactory, const DescriptorPool& dscPool);

	protected:
		ResourceHandle<Model> load_impl(const String& path) const;

	private:
		const UniformBuffer& _uboRef;
		const PipelineCache& _pipelineCacheRef;
		const TextureFactory& _textureFactoryRef;
		const DescriptorPool& _descPoolRef;
	};
}