#pragma once
#include "platform\filesystem.h"

namespace redox {

	class ResourceHelper {
	public:
		ResourceHelper();

		static ResourceHelper& instance() {
			static ResourceHelper instance;
			return instance;
		}

		io::Path resolve_path(const io::Path& file);

	private:
		io::Path _basePath;
	};

}