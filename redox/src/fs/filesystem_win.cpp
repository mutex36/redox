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
#include "core\core.h"

#ifdef RDX_PLATFORM_WINDOWS
#include "filesystem.h"
#include "core\sys\windows.h"

struct redox::File::internal {
	HANDLE handle;
};

redox::File::File(const String& file, const Mode mode) {
	_internal = make_smart_ptr<internal>();

	DWORD access{ 0 };
	if ((mode & Mode::READ) == Mode::READ)
		access |= GENERIC_READ;

	if ((mode & Mode::WRITE) == Mode::WRITE)
		access |= GENERIC_READ;

	_internal->handle = CreateFile(file.cstr(), access, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

redox::File::~File() {
	CloseHandle(_internal->handle);
}

std::size_t redox::File::size() const {
	return GetFileSize(_internal->handle, NULL);
}

redox::Buffer<redox::i8> redox::File::read() {
	Buffer<i8> out(size());

	DWORD dwBytesRead;
	if (!ReadFile(_internal->handle,
		out.data(), out.size(), &dwBytesRead, NULL))
		throw Exception("failed to read file");

	return out;
}

#endif