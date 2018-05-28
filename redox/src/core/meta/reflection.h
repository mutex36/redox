#pragma once
#include "type_hash.h"

#include <type_traits> //std::decay
#include <utility> //std::index_sequence

namespace redox::reflection {
	namespace detail {
		template<size_t I>
		struct ubiq {
			template<typename T>
			constexpr operator T() const noexcept {
				return T{};
			}
		};

		template<size_t I>
		struct ubiq_val {
			size_t& _hash;

			template<typename T>
			constexpr operator T() const noexcept {
				_hash = type_to_hash<std::decay_t<T>>::hash;
				return T{};
			}
		};

		template<typename T, size_t I0, size_t...I>
		constexpr auto count_fields_impl(size_t& out, std::index_sequence<I0, I...>) noexcept
			-> decltype(T{ ubiq<I0>{}, ubiq<I>{}... }) {
			out = sizeof...(I) + 1;
			return {};
		}

		template<typename T, size_t...I>
		constexpr void count_fields_impl(size_t& out, std::index_sequence<I...>) noexcept {
			count_fields_impl<T>(out, std::make_index_sequence<sizeof...(I) - 1>{});
		}

		template<typename T>
		constexpr size_t count_fields() noexcept {
			size_t out = 0;
			count_fields_impl<T>(out, std::make_index_sequence<20>());
			return out;
		}

		template<typename T, size_t...N, size_t N0>
		constexpr void get_field_hash_impl(size_t& out, std::index_sequence<N0, N...>) noexcept {
			T temp{ ubiq<N>{}..., ubiq_val<N0>{out} };
		}

		template<typename T, size_t Index>
		constexpr size_t get_field_hash() noexcept {
			size_t out = 0;
			get_field_hash_impl<T>(out, std::make_index_sequence<Index + 1>{});
			return out;
		}

		template<typename T, size_t Index>
		struct field_type {
			using type = typename 
				hash_to_type<get_field_hash<T, Index>()>::type;
		};

		template<class T, class Fn, size_t...N>
		constexpr void visit_impl(Fn&& fn, std::index_sequence<N...>) noexcept {
			(fn(N, field_type<T, N>{}), ...);
		}

		template<class T, class Fn>
		constexpr void visit(Fn&& fn) noexcept {
			visit_impl<T>(std::forward<Fn>(fn),
				std::make_index_sequence<detail::count_fields<T>()>{});
		}
	}

	template<typename T>
	struct Reflect {
		static_assert(std::is_pod_v<T>, 
			"reflected type is not a POD");

		constexpr auto field_count() const noexcept {
			return detail::count_fields<T>();
		}

		template<class Fn>
		constexpr void visit(Fn&& fn) const noexcept {
			detail::visit<T>(std::forward<Fn>(fn));
		}
	};
}