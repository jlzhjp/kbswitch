#include "kbutil.h"

#include <fstream>

#include "kbdef.h"
#include "winreg.h"

namespace kb {

namespace reg = winreg;

std::unordered_map<std::wstring, std::wstring> get_klid_to_text_map() {
  std::unordered_map<std::wstring, std::wstring> map;
  REGSAM permission = KEY_READ | KEY_QUERY_VALUE;

  reg::RegKey key(HKEY_LOCAL_MACHINE, KB_REG_MACHINE, permission);

  for (std::wstring &subkeyName : key.EnumSubKeys()) {
    reg::RegKey subkey(
        HKEY_LOCAL_MACHINE, KB_REG_MACHINE + subkeyName, permission);
    std::wstring layoutText = subkey.GetStringValue(L"Layout Text");
    map.insert(make_pair(subkeyName, layoutText));
  }

  return map;
}

std::vector<std::wstring> get_user_klid_list() {
  std::vector<std::wstring> res;

  REGSAM permission = KEY_READ | KEY_QUERY_VALUE;

  reg::RegKey key(HKEY_CURRENT_USER, KB_REG_UESR, permission);

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

} // namespace kb
