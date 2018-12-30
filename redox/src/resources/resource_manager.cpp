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
#include "resource_manager.h"
#include "core/application.h"

redox::ResourceManager::ResourceManager(const Path& resourcePath) :
	_resourcePath(io::absolute(resourcePath)) {

	RDX_LOG("Initializing Resource Manager...", ConsoleColor::GREEN);

	auto config = Application::instance->config();

	if (config->get("Resources", "HotReloading")) {
		_monitor.emplace();
		_monitor->subscribe([this](auto file, auto event) {
			std::lock_guard guard(_resourcesMutex);
			_event_resource_modified(file, event);
		});
		_monitor->start(_resourcePath, io::ChangeEvents::FILE_MODIFIED);

		RDX_LOG("Hot-Reload enabled. Monitoring {0}", resourcePath);
	}
}

redox::ResourceManager* redox::ResourceManager::instance() {
	return Application::instance->resource_manager();
}

const redox::Path& redox::ResourceManager::resource_path() const {
	return _resourcePath;
}

void redox::ResourceManager::clear_cache(ResourceGroup groups) {
	RDX_LOG("Clearing resource cache...");
	for (auto it = _cache.begin(); it != _cache.end();) {
		if (util::check_flag(groups, it->second->res_group())) {
			it = _cache.erase(it);
		} else ++it;
	}
}

void redox::ResourceManager::_purge_resources() {
	for (auto it = _cache.begin(); it != _cache.end();) {
		if (it->second.use_count() == 1) {
			it = _cache.erase(it);
		} else ++it;
	}
}

redox::Path redox::ResourceManager::resolve_path(const Path& path) const{
	return _resourcePath / path;
}

void redox::ResourceManager::register_factory(IResourceFactory* factory) {
	_factories.push_back(factory);
}

redox::IResourceFactory* redox::ResourceManager::_find_factory(const Path& ext) {
	for (const auto& fac : _factories) {
		if (fac->supports_ext(ext)) {
			return fac;
		}
	}
	return nullptr;
}

void redox::ResourceManager::_event_resource_modified(const Path& file, io::ChangeEvents event) {
	if (auto cit = _cache.find(file); cit != _cache.end()) {
		RDX_LOG("Resource {0} modified. Attempting to reload...", file);
		auto factory = _find_factory(file.extension());
		
		if (factory) {
			factory->reload(cit->second, file);
		}
	}
}

redox::ResourceHandle<redox::IResource> redox::ResourceManager::load(const redox::Path& path) {
	std::lock_guard guard(_resourcesMutex);

	if (auto cit = _cache.find(path); cit != _cache.end())
		return cit->second;

	auto factory = _find_factory(path.extension());
	if (factory == nullptr)
		throw Exception("not suitable factory found");
	
	RDX_LOG("Loading {0}...", ConsoleColor::WHITE, path);
	auto resource = factory->load(_resourcePath / path);
	_cache[path] = resource;
	return resource;
}
