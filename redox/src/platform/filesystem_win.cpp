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

#include "platform\windows.h"

struct redox::io::File::internal {
	HANDLE handle;
};

redox::io::File::File(const redox::String& file, const Mode mode) :
	_internal(std::make_unique<internal>()) {

	DWORD access{ 0 };
	if (util::check_flag(mode, Mode::READ))
		access |= GENERIC_READ;

	if (util::check_flag(mode, Mode::WRITE))
		access |= GENERIC_WRITE;

	DWORD creationFlags = OPEN_EXISTING;
	if (util::check_flag(mode, Mode::ALWAYS_CREATE))
		creationFlags = CREATE_ALWAYS;

	_internal->handle = CreateFile(file.c_str(), access, 0, NULL,
		creationFlags, FILE_ATTRIBUTE_NORMAL, NULL);

	if (util::check_flag(mode, Mode::THROW_IF_INVALID) && !is_valid())
		throw Exception("failed to open file");
}

redox::io::File::~File() {
	if (is_valid())
		CloseHandle(_internal->handle);
}

bool redox::io::File::is_valid() const {
	return (_internal->handle != INVALID_HANDLE_VALUE);
}

std::size_t redox::io::File::size() const {
	return GetFileSize(_internal->handle, NULL);
}

redox::Buffer<redox::i8> redox::io::File::read() {
	Buffer<i8> out(size());

	DWORD dwBytesRead;
	if (!ReadFile(_internal->handle,
		out.data(), out.size(), &dwBytesRead, NULL))
		throw Exception("failed to read file");

	return out;
}

redox::String redox::io::extension(const String& str) {
	char ext[MAX_PATH];
	_splitpath(str.c_str(), NULL, NULL, NULL, ext);
	return ext;
}

redox::String redox::io::directory(const String& str) {
	char dir[MAX_PATH];
	_splitpath(str.c_str(), NULL, dir, NULL, NULL);
	return dir;
}

redox::String redox::io::filename(const String& str) {
	char fn[MAX_PATH];
	_splitpath(str.c_str(), NULL, NULL, fn, NULL);
	return fn;
}

redox::String redox::io::fullpath(const String& str) {
	TCHAR fp[MAX_PATH];
	GetFullPathName(str.c_str(), MAX_PATH, fp, NULL);
	return fp;
}

redox::String redox::io::tempfile(const String& dir, const String& prefix) {
	_putenv("TMP=");
	auto buffer = _tempnam(dir.c_str(), prefix.c_str());
	String output(buffer);
	std::free(buffer);
	return output;
}

#endif