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

#include "encoding.h"

kbswitch::KeyboardLayoutManager::KeyboardLayoutManager() {}

auto kbswitch::KeyboardLayoutManager::instance() -> KeyboardLayoutManager& {
  static KeyboardLayoutManager instance{};
  return instance;
}

void kbswitch::KeyboardLayoutManager::activate_for_foreground(const std::wstring& klid) const {
  // Load keyboard layout, if KLID is not valid, it will load default layout
  HKL layout = LoadKeyboardLayoutW(klid.c_str(), KLF_ACTIVATE);
  if (!layout) {
    logger().error("Failed to load keyboard layout: {}", wstr2con(klid));
    THROW_LAST_ERROR_MSG("Failed to load keyboard layout");
  }

  // Get foreground window
  HWND hwnd = GetForegroundWindow();
  if (!hwnd) {
    logger().error("Failed to get foreground window");
    THROW_LAST_ERROR_MSG("Failed to get foreground window");
  }

  HRESULT hr = PostMessageW(
    static_cast<HWND>(hwnd),
    WM_INPUTLANGCHANGEREQUEST,
    0,
    reinterpret_cast<LPARAM>(layout));
  if (FAILED(hr)) {
    logger().error("Failed to post input language change request");
    THROW_HR_MSG(hr, "Failed to post input language change request");
  }
}

std::wstring kbswitch::KeyboardLayoutManager::get_current_layout_name() const {
  // KL_NAMELENGTH is 9, but the last character is null terminator
  std::wstring layout_name(KL_NAMELENGTH, L'\0');
  bool success = GetKeyboardLayoutNameW(layout_name.data());
  if (!success) {
    logger().error("Failed to get current keyboard layout name");
    THROW_LAST_ERROR_MSG("Failed to get current keyboard layout name");
  }
  layout_name.resize(KL_NAMELENGTH - 1);
  return layout_name;
}