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

redox::ResourceManager* redox::ResourceManager::instance() {
	return Application::instance->resource_manager();
}

redox::ResourceManager::ResourceManager(const Path& builtinResources, const Path& appResources) :
	_builtinResources(io::absolute(builtinResources)),
	_appResources(io::absolute(appResources)) {

	RDX_LOG("Initializing Resource Manager...", ConsoleColor::GREEN);
	auto config = Application::instance->config();

	if (config->get("Resources", "HotReloading")) {
		_monitor.emplace();
		_monitor->subscribe([this](auto file, auto event) {
			_event_resource_modified(file, event);
		});
		_monitor->start(_appResources, io::ChangeEvents::FILE_MODIFIED);
		RDX_LOG("Hot-Reload enabled. Monitoring App resources...");
	}
}

void redox::ResourceManager::clear_cache(ResourceGroup groups) {
	RDX_LOG("Clearing resource cache...");
	for (auto it = _cache.begin(); it != _cache.end();) {
		if (util::check_flag(groups, it->second->res_group())) {
			it = _cache.erase(it);
		} else ++it;
	}
}

redox::Path redox::ResourceManager::resolve_path(const Path& path) const {
	if (auto id = path.string(); id.find("builtin:") == 0) {
		return _builtinResources / id.substr(8);
	}
	return _appResources / path;
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
	std::lock_guard guard(_resourcesMutex);
	if (auto cit = _cache.find(file); cit != _cache.end()) {
		RDX_LOG("Resource {0} modified. Attempting to reload...", file);
		auto nr = load(file);
		onReloadResource(cit->second, nr);
	}
}

redox::ResourceHandle<redox::IResource> redox::ResourceManager::load(const Path& path) {
	auto resolvedPath = resolve_path(path);
	if (!io::is_regular_file(resolvedPath)) {
		RDX_LOG("Resource does not exist: {0}", ConsoleColor::RED, path);
		return nullptr;
	}

	RDX_LOG("Loading {0}...", ConsoleColor::WHITE, path);
	RDX_UNUSED(std::lock_guard(_resourcesMutex));

	if (auto cit = _cache.find(resolvedPath); cit != _cache.end()) {
		return cit->second;
	}

	auto factory = _find_factory(resolvedPath.extension());
	if (factory == nullptr) {
		throw Exception(redox::format("no suitable factory found for {0}",
			resolvedPath.extension()));
	}

	auto resource = factory->load(resolvedPath);
	if (resource) {
		_cache[resolvedPath] = resource;
	}
	return resource;
}

redox::ResourceHandle<redox::IResource> redox::ResourceManager::load(const Path& path, const Path& fallback) {
	
	auto resource = load(path);
	if (resource) {
		return resource;
	}

	auto fallbackResource = load(fallback);
	if (fallbackResource) {
		return fallbackResource;
	}

	throw Exception("failed to load resources.");
}
