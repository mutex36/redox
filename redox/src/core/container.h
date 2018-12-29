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
#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <array>
#include <filesystem>

#include <thirdparty/function_ref/function_ref.hpp>

namespace redox
{
	template<class T>
	using Buffer = std::vector<T>;

	template<class Key, class Value>
	using Hashmap = std::unordered_map<Key, Value>;

	using String = std::string;
	using WString = std::wstring;
	using StringView = std::string_view;

	template<class T>
	using UniquePtr = std::unique_ptr<T>;

	using std::make_unique;

	template<class T>
	using SharedPtr = std::shared_ptr<T>;

	using std::make_shared;

	template<class S>
	using Function = std::function<S>;

	template<class S>
	using FunctionRef = tl::function_ref<S>;

	template<class T, std::size_t N>
	using Array = std::array<T, N>;

	using Path = std::filesystem::path;
}

namespace std {
	template<>
	struct hash<std::filesystem::path> {
		std::size_t operator()(const std::filesystem::path& path) const {
			return std::filesystem::hash_value(path);
		}
	};
}