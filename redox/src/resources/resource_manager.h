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
#include <core/core.h>
#include <core/non_copyable.h>
#include <resources/resource.h>
#include <platform/filesystem.h>
#include <core/logging/log.h>
#include <core/event.h>

#include <platform/filesystem.h>
#include <mutex> //std::mutex, std::lock_guard
#include <optional> //std::optional

namespace redox {
	class ResourceManager : public NonCopyable {
	public:
		static ResourceManager* instance();
			
		ResourceManager(const Path& builtinResources, const Path& appResources);
		~ResourceManager() = default;

		void clear_cache(ResourceGroup groups);
		void register_factory(IResourceFactory* factory);

		ResourceHandle<IResource> load(const Path& path);
		ResourceHandle<IResource> load(const Path& path, const Path& fallback);
		Path resolve_path(const Path& path) const;

		template<class R, class...Args>
		ResourceHandle<R> load(Args&&...args) {
			static_assert(std::is_base_of_v<IResource, R>, "<R> must be of type IResource");
			return std::static_pointer_cast<R>(load(std::forward<Args>(args)...));
		}

		Event<ResourceHandle<IResource>, ResourceHandle<IResource>> onReloadResource;

	private:
		IResourceFactory* _find_factory(const Path& ext);
		void _event_resource_modified(const Path& file, io::ChangeEvents event);

		Path _appResources;
		Path _builtinResources;

		std::recursive_mutex _resourcesMutex;
		Hashmap<Path, ResourceHandle<IResource>> _cache;
		Buffer<IResourceFactory*> _factories;
		std::optional<io::DirectoryWatcher> _monitor;
	};
}