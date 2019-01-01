#pragma once
#include <core\core.h>
#include <core\non_copyable.h>

namespace redox {

	template<class...Signature>
	class Event : public NonCopyable {
	public:
		using FnType = Function<void(Signature...)>;

		Event& operator+=(FnType&& fn) {
			_subscriber.push_back(std::move(fn));
			return *this;
		}

		Event& operator+=(const FnType& fn) {
			_subscriber.push_back(fn);
			return *this;
		}

		template<class...Args>
		void fire(Args&&...args) {
			for (const auto& fn : _subscriber) {
				fn(std::forward<Args>(args)...);
			}
		}
		
	private:
		Buffer<FnType> _subscriber;
	};
}