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

redox::io::File::File(const redox::Path& file, const Mode mode) :
	_internal(std::make_unique<internal>()) {

	DWORD access{ 0 };
	if (util::check_flag(mode, Mode::READ))
		access |= GENERIC_READ;

	if (util::check_flag(mode, Mode::WRITE))
		access |= GENERIC_WRITE;

	DWORD creationFlags = OPEN_EXISTING;
	if (util::check_flag(mode, Mode::ALWAYS_CREATE))
		creationFlags = CREATE_ALWAYS;

	_internal->handle = CreateFileW(file.c_str(), access, 0, NULL,
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
		out.data(), static_cast<DWORD>(out.size()), &dwBytesRead, NULL))
		throw Exception("failed to read file");

	return out;
}

// DirectoryWatcher

struct redox::io::DirectoryWatcher::internal {
	bool running{ false };
	Path directory;
	ChangeEvents events;
	CallbackType callback;
	HANDLE directoryHandle;
	HANDLE waitHandle;
	OVERLAPPED overlapped;
	Buffer<i8> buffer;

	void read_changes();
	static VOID CALLBACK WaitCallback(
		_In_ PVOID   lpParameter,
		_In_ BOOLEAN TimerOrWaitFired);
};

redox::io::DirectoryWatcher::DirectoryWatcher() :
	_internal(std::make_unique<internal>()) {
}

redox::io::DirectoryWatcher::~DirectoryWatcher() {
	stop();
}

void redox::io::DirectoryWatcher::subscribe(CallbackType callback) {
	_internal->callback = std::move(callback);
}

void redox::io::DirectoryWatcher::start(const Path& directory, ChangeEvents events) {
	stop();

	_internal->directory = directory;
	_internal->events = events;
	_internal->directoryHandle = CreateFileW(
		directory.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);

	if (_internal->directoryHandle == INVALID_HANDLE_VALUE) {
		throw Exception("CreateFileW() failed.");
	}

	constexpr auto bfSize = 10 * (sizeof(FILE_NOTIFY_INFORMATION));
	_internal->overlapped.hEvent = CreateEvent(0, 0, 0, 0);
	_internal->buffer.reserve(static_cast<size_t>(bfSize));

	if (!RegisterWaitForSingleObject(
		&_internal->waitHandle,
		_internal->overlapped.hEvent,
		internal::WaitCallback,
		this, INFINITE, WT_EXECUTEDEFAULT)) {

		throw Exception("RegisterWaitForSingleObject() failed.");
	}

	_internal->running = true;
	_internal->read_changes();
}

void redox::io::DirectoryWatcher::stop() {
	if (_internal->running) {
		UnregisterWaitEx(_internal->waitHandle, NULL);
		CloseHandle(_internal->overlapped.hEvent);
		CloseHandle(_internal->directoryHandle);
		_internal->running = false;
	}
}

void redox::io::DirectoryWatcher::internal::read_changes() {
	buffer.clear();

	auto filter = FILE_NOTIFY_CHANGE_FILE_NAME;
	if (util::check_flag(events, ChangeEvents::FILE_MODIFIED)) {
		filter |= FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS |
			FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY;
	}

	[[maybe_unused]] DWORD bytes;
	if (!ReadDirectoryChangesW(
		directoryHandle, buffer.data(), static_cast<DWORD>(buffer.capacity()),
		TRUE, filter, &bytes, &overlapped, NULL)) {

		throw Exception("ReadDirectoryChangesW() failed.");
	}
}

VOID CALLBACK redox::io::DirectoryWatcher::internal::WaitCallback(
	_In_ PVOID lpParameter, _In_ BOOLEAN TimerOrWaitFired) {

	auto instance = static_cast<DirectoryWatcher*>(lpParameter);
	auto& internal = *instance->_internal;
	auto fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(internal.buffer.data());

	if (internal.callback) {

		ChangeEvents event{ ChangeEvents::UNKNOWN };
		if (util::check_flag(fni->Action, FILE_ACTION_ADDED)
			&& util::check_flag(internal.events, ChangeEvents::FILE_ADDED)) {
			event |= ChangeEvents::FILE_ADDED;
		}

		if (util::check_flag(fni->Action, FILE_ACTION_MODIFIED)
			&& util::check_flag(internal.events, ChangeEvents::FILE_MODIFIED)) {
			event |= ChangeEvents::FILE_MODIFIED;
		}

		if (util::check_flag(fni->Action, FILE_ACTION_REMOVED)
			&& util::check_flag(internal.events, ChangeEvents::FILE_REMOVED)) {
			event |= ChangeEvents::FILE_REMOVED;
		}

		if (util::check_flag(fni->Action, FILE_ACTION_RENAMED_OLD_NAME)
			&& util::check_flag(internal.events, ChangeEvents::FILE_RENAME)) {
			event |= ChangeEvents::FILE_REMOVED;
		}

		if (event != ChangeEvents::UNKNOWN) {
			WString wstr{ fni->FileName, fni->FileNameLength / sizeof(WCHAR) };
			Path path(wstr);
			if (io::is_regular_file(internal.directory / path)) {
				internal.callback(path, event);
			}
		}
	}

	internal.read_changes();
}

#endif