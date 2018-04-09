#pragma once
#include "core\core.h"
#include "core\string.h"
#include "memory\smart_ptr.h"

#include <stdlib.h> //std::strtof(), std::strtoll()

namespace redox {

	template<class T>
	T parse(const String& expr) { static_assert(0); }

	template<>
	f32 parse(const String& expr) {
		return std::strtof(expr.cstr(), NULL);
	}

	template<>
	i64 parse(const String& expr) {
		return std::strtoll(expr.cstr(), NULL, 10);
	}

	template<class...Args>
	String string_format(const String& format, const Args&...args) {
		i8 s0;
		String out(format.size());
		for (std::size_t i = 0; i < format.size(); ++i) {

			if (format[i] == '{') {
				s0 = i; continue;
			}

			if (format[i] == '}') {
				if (s0 == -1) throw;

				auto idStr = format.substr(s0 + 1, i - 1);
				auto id = parse<i64>(idStr);

				
				s0 = -1;
				continue;
			}

		}

		if (s0 != -1) throw;

		return {};
	}


}