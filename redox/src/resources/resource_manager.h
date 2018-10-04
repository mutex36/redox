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
#include "core\core.h"
#include "core\non_copyable.h"
#include "resource.h"

#include "platform/filesystem.h"

namespace redox {

	class ResourceManager : public NonCopyable {
	public:
		static const ResourceManager& instance();

		ResourceManager();
		String resolve_path(const String& path) const;
		void register_factory(IResourceFactory* factory) const;

		template<class R>
		ResourceHandle<R> load(const String& path) const {
			static_assert(std::is_base_of_v<IResource, R>);

			auto cit = _cache.find(path);
			if (cit != _cache.end())
				return std::static_pointer_cast<R>(cit->second);

			for (const auto& fac : _factories)
				if (fac->supports_ext(io::extension(path)))
					return std::static_pointer_cast<R>(fac->load(_resourcePath + path));

			throw Exception("not suitable factory found");
		}

	private:
		String _resourcePath;

		mutable Hashmap<String, ResourceHandle<IResource>> _cache;
		mutable Buffer<IResourceFactory*> _factories;
	};
}