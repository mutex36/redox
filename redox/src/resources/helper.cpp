#include "helper.h"

redox::ResourceHelper::ResourceHelper() 
	: _basePath(R"(C:\Users\luis9\Desktop\redox\redox\resources\)") {
}

redox::io::Path redox::ResourceHelper::resolve_path(const io::Path& file) {
	return _basePath + file;
}
