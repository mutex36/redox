#pragma once
#include <core\core.h>
#include <core\non_copyable.h>

namespace redox {

	template<class...Signature>
	class Event : public NonCopyable {
	public:
		using FnType = Function<void(Signature...)>;

		template<class Fn>
		Event& operator+=(Fn&& fn) {
			_subscriber.emplace_back(std::forward<Fn>(fn));
			return *this;
		}

		template<class...Args>
		void operator()(Args&&...args) {
			for (const auto& fn : _subscriber) {
				fn(std::forward<Args>(args)...);
			}
		}
		
	private:
		Buffer<FnType> _subscriber;
	};
}