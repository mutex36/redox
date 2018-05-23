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
#include "core\string.h"
#include "core\hashmap.h"
#include "platform\filesystem.h"
#include "core\ref_counted.h"

#include "helper.h"

#include <type_traits> //std::forward

namespace redox {
	template<class T>
	using Resource = RefCounted<T>;

	template<class Derived, class ResourceType>
	class ResourceFactory {
	public:
		template<class...Args>
		Resource<ResourceType> load(const io::Path& file, Args&&...args) {
			auto lookup = _cache.get(file);
			if (lookup) 
				return lookup.value();

			auto path = ResourceHelper::instance().resolve_path(file);
			auto instance = static_cast<Derived*>(this);
			auto rx = instance->load_impl(path, std::forward<Args>(args)...);

			_cache.push(file, rx);
			return rx;
		}

	private:
		Hashmap<String, Resource<ResourceType>> _cache;
	};
}