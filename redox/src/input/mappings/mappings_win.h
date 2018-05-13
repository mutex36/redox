#pragma once
#include "core\core.h"

#ifdef RDX_PLATFORM_WINDOWS
#include "core\hashmap.h"
#include "input\keys.h"

#include "platform\windows.h"

namespace redox::input {
	//http://www.kbdedit.com/manual/low_level_vk_list.html
	static const redox::Hashmap<UINT, Keys> g_vkey_mappings{
		{ 0x41, Keys::A },
		{ 0x42, Keys::B },
		{ 0x43, Keys::C },
		{ 0x44, Keys::D },
		{ 0x45, Keys::E },
		{ 0x46, Keys::F },
		{ 0x47, Keys::G },
		{ 0x48, Keys::H },
		{ 0x49, Keys::I },
		{ 0x4A, Keys::J },
		{ 0x4B, Keys::K },
		{ 0x4C, Keys::L },
		{ 0x4D, Keys::M },
		{ 0x4E, Keys::N },
		{ 0x4F, Keys::O },
		{ 0x50, Keys::P },
		{ 0x51, Keys::Q },
		{ 0x52, Keys::R },
		{ 0x53, Keys::S },
		{ 0x54, Keys::T },
		{ 0x55, Keys::U },
		{ 0x56, Keys::V },
		{ 0x57, Keys::W },
		{ 0x58, Keys::X },
		{ 0x59, Keys::Y },
		{ 0x5A, Keys::Z }
	};
}
#endif