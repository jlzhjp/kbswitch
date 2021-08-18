#pragma once

#include <Windows.h>

#include <filesystem>
#include <unordered_map>

namespace kb {

namespace fs = std::filesystem;

[[nodiscard]] std::unordered_map<std::wstring, std::wstring>
    get_klid_to_text_map();

// Get installed Keyboard Layouts
[[nodiscard]] std::vector<std::wstring> get_user_klid_list();

[[nodiscard]] std::wstring get_current_klid();

void set_keyboard_layout(HWND hwnd, const std::wstring &klid);

void cache_keyborad_layout_state();

// Return: If restore success
bool restore_keyboard_layout_state();

} // namespace kb
