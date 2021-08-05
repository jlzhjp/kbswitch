#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

#include <Windows.h>

#include "winreg.hpp"

namespace reg = winreg;

namespace fs = std::filesystem;

// input locale identifer = HKL (keyboard layout handle)

void writeln(const std::wstring &text = L"", DWORD handle = STD_OUTPUT_HANDLE);

void writelnerr(const std::wstring &text);

std::vector<std::wstring> convert_args(int argc, wchar_t **argv);

void print_help();

std::unordered_map<std::wstring, std::wstring> get_klid_to_text_map();

std::vector<std::wstring> get_user_klid_list();

void set_keyboard_layout(HWND hwnd, const std::wstring &klid);

fs::path get_temp_file_path();

void cache_keyborad_layout_state();

bool restore_keyboard_layout_state();

int wmain(int argc, wchar_t **argv) {
  std::vector<std::wstring> args = convert_args(argc, argv);

  if (argc <= 1) {
    writelnerr(L"[Error] No command.");
    print_help();
    return -1;
  }

  auto iter = ++args.begin();

  if (*iter == L"cache") {
    // cache

    cache_keyborad_layout_state();

  } else if (*iter == L"restore") {
    // restore

    if (!restore_keyboard_layout_state()) {
      writelnerr(L"[Error] No cache.");
    }

  } else if (*iter == L"list") {
    // list

    std::vector<std::wstring> keyboardList = get_user_klid_list();
    std::unordered_map<std::wstring, std::wstring> map = get_klid_to_text_map();

    for (int i = 0; i != keyboardList.size(); ++i) {
      writeln(std::to_wstring(i) + L"\t" + map[keyboardList[i]]);
    }

  } else if (*iter == L"set") {
    // set [number]

    auto nextIter = ++iter;

    if (nextIter == args.end()) {
      writelnerr(L"[Error] set: No number provided.");
      return -1;
    }

    int number;

    try {
      number = std::stoi(*nextIter);
    } catch (const std::invalid_argument &) {
      writelnerr(L"[Error] set <number>: Invalid number " + *nextIter + L".");
      return -1;
    }

    std::vector<std::wstring> keyboardList = get_user_klid_list();

    if (number < 0 || static_cast<std::size_t>(number) >= keyboardList.size()) {
      writelnerr(L"[Error] set <number>: Out of range.");
      writelnerr(L"The valid range is [0," +
                 std::to_wstring(keyboardList.size()) + L").");
      return -1;
    }

    set_keyboard_layout(GetForegroundWindow(), keyboardList[number]);

  } else if (*iter == L"help") {
    // help

    print_help();

  } else {

    writelnerr(L"[Error] Unknown command: " + *iter + L".");
    return -1;
  }

  if (++iter != args.end()) {
    writeln(L"[Warning] Too many arguments, some arguments are ignored.");
  }
  return 0;
}

void writeln(const std::wstring &text, DWORD handle) {
  auto hOutput = GetStdHandle(handle);
  WriteConsole(
      hOutput, text.c_str(), static_cast<int>(text.size()), nullptr, nullptr);
  WriteConsole(hOutput, L"\n", 1, nullptr, nullptr);
}

void writelnerr(const std::wstring &text) {
  writeln(text, STD_ERROR_HANDLE);
}

std::vector<std::wstring> convert_args(int argc, wchar_t **argv) {
  std::vector<std::wstring> res;
  for (int i = 0; i < argc; ++i) {
    res.push_back(argv[i]);
  }
  return res;
}

void print_help() {
  writeln(L"usage: kbswitch <command> [<args>]");
  writeln();
  writeln(L"available commands:");
  writeln(L"list          list all installed keyboard layouts");
  writeln(L"set <number>  switch to the given keyboard layout");
  writeln(L"cache         save the current keyboard layout state");
  writeln(L"restore       restore to the saved state");
  writeln(L"help          show this help message");
}

std::unordered_map<std::wstring, std::wstring> get_klid_to_text_map() {
  std::wstring regPath =
      L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\";

  std::unordered_map<std::wstring, std::wstring> map;
  auto permission = KEY_READ | KEY_QUERY_VALUE;

  reg::RegKey key(HKEY_LOCAL_MACHINE, regPath, permission);

  for (std::wstring &subkeyName : key.EnumSubKeys()) {
    reg::RegKey subkey(HKEY_LOCAL_MACHINE, regPath + subkeyName, permission);
    std::wstring layoutText = subkey.GetStringValue(L"Layout Text");
    map.insert(make_pair(subkeyName, layoutText));
  }

  return map;
}

std::vector<std::wstring> get_user_klid_list() {
  std::vector<std::wstring> res;

  std::wstring regPath = L"Keyboard Layout\\Preload\\";
  auto permission = KEY_READ | KEY_QUERY_VALUE;

  reg::RegKey key(HKEY_CURRENT_USER, regPath, permission);

  for (auto &p : key.EnumValues()) {
    res.push_back(key.GetStringValue(p.first));
  }

  return res;
}

void set_keyboard_layout(HWND hwnd, const std::wstring &klid) {
  // get HKL from KLID
  HKL keyboard = LoadKeyboardLayout(klid.c_str(), KLF_ACTIVATE);
  PostMessage(
      hwnd, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(keyboard));
}

fs::path get_temp_file_path() {
  fs::path tempDir = fs::temp_directory_path();
  fs::path tempFile(L"_kbswitch_temp");
  return tempDir / tempFile;
}

void cache_keyborad_layout_state() {
  auto tempFilePath = get_temp_file_path();

  std::unique_ptr<wchar_t[]> buffer(new wchar_t[KL_NAMELENGTH]);
  GetKeyboardLayoutName(buffer.get());

  // To avoid unnecessary disk writes
  if (fs::exists(tempFilePath)) {
    std::wifstream ifs(tempFilePath);
    std::wstring cache;
    ifs >> cache;
    if (cache == buffer.get()) {
      return;
    }
  }

  std::wofstream ofs(tempFilePath);
  ofs << buffer.get();
}

bool restore_keyboard_layout_state() {
  auto tempFilePath = get_temp_file_path();

  if (!fs::exists(tempFilePath)) {
    return false;
  }
  std::wifstream ifs(tempFilePath);

  std::wstring buffer;
  ifs >> buffer;

  set_keyboard_layout(GetForegroundWindow(), buffer);

  return true;
}
