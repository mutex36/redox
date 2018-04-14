#pragma once
#include "core\core.h"

namespace redox::allocation {
	struct LinearGrowth {
		std::size_t operator()(std::size_t in) {
			return in + 1;
		}
	};
	struct ExponentialGrowth {
		std::size_t operator()(std::size_t in) {
			if (in == 0)
				return 2;
			return in * 2;
		}
	};
	using DefaultGrowth = ExponentialGrowth;
}
