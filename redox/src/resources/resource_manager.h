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
#include "core\hashmap.h"
#include "resource.h"

#include "core/meta/type_hash.h"

namespace redox {

	class ResourceManager {
	public:
		ResourceManager(const String& resourcePath);
		String resolve_path(const String& path) const;

		template<class R>
		void register_factory(IResourceFactory* factory) const {
			_factories.push(
				redox::reflection::type_to_hash<R>::hash, factory);
		}

		template<class R>
		ResourceHandle<R> load(const String& path) const {
			auto cit = _cache.get(path);
			if (cit != _cache.end())
				return std::static_pointer_cast<R>(cit->value);

			auto fit = _factories.get(redox::reflection::type_to_hash<R>::hash);
			if (fit == _factories.end())
				throw Exception("no suitable factory found");

			return fit->value->load(resolve_path(path));
		}

	private:
		String _resourcePath;

		mutable Hashmap<String, ResourceHandle<IResource>> _cache;
		mutable Hashmap<redox::reflection::hash_type, IResourceFactory*> _factories;
	};
}