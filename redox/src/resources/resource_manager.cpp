#include "resource_manager.h"

#include "core/application.h"

redox::ResourceManager::ResourceManager() :
	_cache("0"),
	_resourcePath("resources\\") {
}

const redox::ResourceManager& redox::ResourceManager::instance() {
	return Application::instance->resource_manager();
}

redox::String redox::ResourceManager::resolve_path(const String& path) const{
	return _resourcePath + path;
}

void redox::ResourceManager::register_factory(IResourceFactory* factory) const {
	_factories.push(factory);
}
