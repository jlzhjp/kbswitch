#include "keyboard_layout_manager.h"

#include <print>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <wil/resource.h>
#include <wil/result.h>

kbswitch::KeyboardLayoutManager::KeyboardLayoutManager() {}

auto kbswitch::KeyboardLayoutManager::instance() -> KeyboardLayoutManager& {
  static KeyboardLayoutManager instance{};
  return instance;
}

void kbswitch::KeyboardLayoutManager::activate_for(void* hwnd, const std::wstring& klid) const {
  wil::last_error_context ctx;
  HKL layout = LoadKeyboardLayoutW(klid.c_str(), KLF_ACTIVATE);
  THROW_LAST_ERROR_IF(!layout);
  HRESULT hr = PostMessageW(
    static_cast<HWND>(hwnd),
    WM_INPUTLANGCHANGEREQUEST,
    0,
    reinterpret_cast<LPARAM>(layout));
  THROW_IF_FAILED(hr);
}

std::wstring kbswitch::KeyboardLayoutManager::get_current_layout_name() const {
  // KL_NAMELENGTH is 9, but the last character is null terminator
  std::wstring layout_name(KL_NAMELENGTH, L'\0');
  bool success = GetKeyboardLayoutNameW(layout_name.data());
  THROW_LAST_ERROR_IF(!success);
  layout_name.resize(KL_NAMELENGTH - 1);
  return layout_name;
}