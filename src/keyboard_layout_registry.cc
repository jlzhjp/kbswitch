#include "keyboard_layout_registry.h"

#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Shlwapi.h>
#include <Windows.h>

#include <wil/stl.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <wil/registry.h>
#include <wil/win32_helpers.h>

#include "encoding.h"

kbswitch::KeyboardLayoutPreloadRegistry::KeyboardLayoutPreloadRegistry() {
  wil::unique_hkey target_hkey;

  try {
    // open the Keyboard Layouts registry key
    target_hkey = wil::reg::create_unique_key(HKEY_CURRENT_USER, L"Keyboard Layout\\Preload");
  } catch (const std::exception& ex) {
    logger().error("Failed to get `Keyboard Layout\\Preload` registry key, Error: {}", ex.what());
    throw;
  }

  wil::reg::value_iterator value_it{target_hkey.get()};
  std::vector<KeyboardLayoutPreloadEntry> entries;

  try {
    // iterate over all keyboard layouts
    for (const auto& key : wil::make_range(value_it, wil::reg::value_iterator{})) {
      auto& index = key.name;
      auto klid = wil::reg::get_value<std::wstring>(target_hkey.get(), index.c_str());

      // find substitute for the keyboard layout KLID if it exists
      auto substitute = find_substitute(klid);
      if (substitute.has_value()) {
        logger().info("Substitute found for {}: {}", wstr2con(klid), wstr2con(substitute.value()));
      }
      entries.push_back(KeyboardLayoutPreloadEntry{
        .index = index,
        .klid = substitute.value_or(klid),
      });
    }
  } catch (const std::exception& ex) {
    logger().error("Failed to query `Keyboard Layout\\Preload`, Error: {}", ex.what());
    throw;
  }

  logger().info("`Keyboard Layout\\Preload` Found {} keyboard layouts", entries.size());
  _entries = std::move(entries);
}

auto kbswitch::KeyboardLayoutPreloadRegistry::instance() -> KeyboardLayoutPreloadRegistry& {
  static KeyboardLayoutPreloadRegistry instance;
  return instance;
}

auto kbswitch::KeyboardLayoutPreloadRegistry::entries() const
  -> const std::vector<KeyboardLayoutPreloadEntry>& {
  return _entries;
}

std::optional<std::wstring>
  kbswitch::KeyboardLayoutPreloadRegistry::find_substitute(const std::wstring& klid) const {
  wil::unique_hkey target_hkey;
  if (FAILED(wil::reg::create_unique_key_nothrow(
        HKEY_CURRENT_USER,
        L"Keyboard Layout\\Substitutes",
        target_hkey))) {
    logger().info(
      "No substitutes in `Keyboard Layout\\Substitutes` registry key, KLID: {}",
      wstr2con(klid));
    return std::nullopt;
  }
  return wil::reg::try_get_value<std::wstring>(target_hkey.get(), klid.c_str());
}

auto kbswitch::KeyboardLayoutPreloadEntry::query_info() const -> KeyboardLayoutEntry {
  // open the Keyboard Layouts registry key
  static const auto target_hkey = wil::reg::create_unique_key(
    HKEY_LOCAL_MACHINE,
    L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts");

  // open subkey for each keyboard layout
  auto kl_hkey = wil::reg::open_unique_key(target_hkey.get(), klid.c_str());

  std::wstring layout_text = wil::reg::get_value<std::wstring>(kl_hkey.get(), L"Layout Text");
  std::wstring layout_file = wil::reg::get_value<std::wstring>(kl_hkey.get(), L"Layout File");

  std::wstring display_name_resource
    = wil::reg::get_value<std::wstring>(kl_hkey.get(), L"Layout Display Name");

  std::wstring layout_display_name;
  HRESULT hr = wil::AdaptFixedSizeToAllocatedResult<std::wstring, 256>(
    layout_display_name,
    [&display_name_resource](PWSTR value, size_t value_size, size_t* next_size) {
      HRESULT hr = SHLoadIndirectString(display_name_resource.c_str(), value, value_size, nullptr);
      if (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
        if (value_size > 65535) return hr;
        *next_size = value_size * 2;
        return S_OK;
      }
      *next_size = wcsnlen_s(value, value_size) + 1;
      return hr;
    });

  if (SUCCEEDED(hr)) {
    layout_display_name.resize(wcsnlen_s(layout_display_name.c_str(), layout_display_name.size()));
  } else {
    logger().warn("Failed to load display name string for KLID: {}, Error: {}", wstr2con(klid), hr);
    layout_display_name = L"<Unknown>";
  }

  return KeyboardLayoutEntry{this->klid, layout_text, layout_file, layout_display_name};
}
