#include "resource_manager.h"

redox::ResourceManager::ResourceManager(const String& resourcePath) :
	_cache("0"),
	_factories(0xFFFF),
	_resourcePath(resourcePath) {
}

redox::String redox::ResourceManager::resolve_path(const String& path) const{
	return _resourcePath + path;
}
