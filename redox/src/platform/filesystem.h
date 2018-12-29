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
#include <core\core.h>
#include <core\non_copyable.h>
#include <core\utility.h>

namespace redox::io {
	class File : public NonCopyable {
	public:
		enum class Mode {
			READ = 0x1 << 0,
			WRITE = 0x1 << 1,
			ALWAYS_CREATE = 0x1 << 2,
			THROW_IF_INVALID = 0x1 << 3
		};

		File(const Path& file, const Mode mode = Mode::READ);
		~File();

		bool is_valid() const;
		std::size_t size() const;
		Buffer<i8> read();

	private:
		struct internal;
		UniquePtr<internal> _internal;
	};

	enum class ChangeEvents {
		FILE_ADDED = 0x1 << 0,
		FILE_REMOVED = 0x1 << 1,
		FILE_MODIFIED = 0x1 << 2,
		FILE_RENAME = 0x1 << 3,
		UNKNOWN = 0x1 << 4
	};

	class DirectoryWatcher : public NonCopyable {
	public:
		using CallbackType = Function<void(const Path& file, ChangeEvents events)>;

		DirectoryWatcher();
		~DirectoryWatcher();

		void subscribe(CallbackType callback);
		void start(const Path& directory, ChangeEvents events);
		void stop();

	private:
		struct internal;
		UniquePtr<internal> _internal;
	};

	using namespace std::filesystem;
}

RDX_ENABLE_ENUM_FLAGS(::redox::io::File::Mode);
RDX_ENABLE_ENUM_FLAGS(::redox::io::ChangeEvents);