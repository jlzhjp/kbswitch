#include "command_executor.h"

#include <filesystem>
#include <print>
#include <string>
#include <fstream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "encoding.h"
#include "keyboard_layout_manager.h"
#include "keyboard_layout_registry.h"

const auto TEMP_FILE =
    std::filesystem::temp_directory_path() / L"_kbswitch_temp";
const auto LOG_FILE = std::filesystem::temp_directory_path() / L"_kbswitch_log";

void kbswitch::CommandExecutor::query_current_active_layout() const {
  try {
    auto &_layout_manager = KeyboardLayoutManager::instance();
    auto current_layout = _layout_manager.get_current_layout_name();
    std::println("{}", wstr2con(current_layout));
  } catch (const std::exception &ex) {
    logger().error("Error: {}", ex.what());
  }
}

kbswitch::CommandExecutor::CommandExecutor()
    : _std_output{(void *)GetStdHandle(STD_OUTPUT_HANDLE)} {}

void kbswitch::CommandExecutor::query_keyboard_layouts() const {
  try {
    auto &_registry = KeyboardLayoutPreloadRegistry::instance();

    static constexpr auto output_format = "{:^8}{:^16}    {:<28}{:^8}";
    std::wstring current_klid =
        KeyboardLayoutManager::instance().get_current_layout_name();
    logger().info("Current active keyboard layout: {}", wstr2con(current_klid));

    SetConsoleTextAttribute(_std_output, FOREGROUND_GREEN);
    std::println(output_format, "Index", "KLID", "Layout Name", "Status");
    std::println(output_format, "-----", "----", "-----------", "------");
    SetConsoleTextAttribute(_std_output, 0x0F);

    for (const auto &layout : _registry.entries()) {
      auto info = layout.query_info();

      std::print("{:^8}", wstr2con(layout.index));
      std::print("{:^16}", wstr2con(layout.klid));
      std::print("    {:28}", wstr2con(info.layout_display_name));

      SetConsoleTextAttribute(_std_output, FOREGROUND_GREEN);
      std::println("{:^8}", current_klid == layout.klid ? "*Active*" : "");
      SetConsoleTextAttribute(_std_output, 0x0F);
    }
  } catch (const std::exception &ex) {
    logger().error("Error: {}", ex.what());
  }
}

void kbswitch::CommandExecutor::activate_keyboard_layout(const std::string& klid) const {
  try {
    auto &_layout_manager = KeyboardLayoutManager::instance();
    
    // Convert input to wstring
    std::wstring wklid(klid.begin(), klid.end());
    
    // Get foreground window
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
      logger().error("Failed to get foreground window");
      std::println("Failed to get foreground window");
      return;
    }

    // Switch keyboard layout
    _layout_manager.activate_for(hwnd, wklid);
    std::println("Keyboard layout set to: {}", klid);
  } catch (const std::exception &ex) {
    logger().error("Error setting keyboard layout: {}", ex.what());
    std::println("Error setting keyboard layout: {}", ex.what());
  }
}

void kbswitch::CommandExecutor::cache_keyboard_layout() const {
  try {
    auto &_layout_manager = KeyboardLayoutManager::instance();
    auto current_klid = _layout_manager.get_current_layout_name();
    
    // Write to temp file
    std::wofstream cache_file(TEMP_FILE);
    if (!cache_file.is_open()) {
      logger().error("Failed to open cache file for writing");
      std::println("Failed to cache keyboard layout");
      return;
    }
    
    cache_file << current_klid;
    cache_file.close();
    
    std::println("Current keyboard layout cached: {}", wstr2con(current_klid));
  } catch (const std::exception &ex) {
    logger().error("Error caching keyboard layout: {}", ex.what());
    std::println("Error caching keyboard layout: {}", ex.what());
  }
}

void kbswitch::CommandExecutor::restore_keyboard_layout() const {
  try {
    // Check if cache file exists
    if (!std::filesystem::exists(TEMP_FILE)) {
      logger().error("No cached keyboard layout found");
      std::println("No cached keyboard layout found");
      return;
    }
    
    // Read from temp file
    std::wifstream cache_file(TEMP_FILE);
    if (!cache_file.is_open()) {
      logger().error("Failed to open cache file for reading");
      std::println("Failed to restore keyboard layout");
      return;
    }
    
    std::wstring cached_klid;
    std::getline(cache_file, cached_klid);
    cache_file.close();
    
    // Get foreground window
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
      logger().error("Failed to get foreground window");
      std::println("Failed to get foreground window");
      return;
    }
    
    // Restore layout
    auto &_layout_manager = KeyboardLayoutManager::instance();
    _layout_manager.activate_for(hwnd, cached_klid);
    
    std::println("Keyboard layout restored to: {}", wstr2con(cached_klid));
  } catch (const std::exception &ex) {
    logger().error("Error restoring keyboard layout: {}", ex.what());
    std::println("Error restoring keyboard layout: {}", ex.what());
  }
}
