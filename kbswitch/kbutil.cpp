#include "kbutil.h"

#include <fstream>

#include "kbdef.h"
#include "winreg.h"

// input locale identifer = HKL (keyboard layout handle)

namespace kb {

namespace reg = winreg;

constexpr auto KB_REG_MACHINE =
    L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\";

constexpr auto KB_REG_UESR = L"Keyboard Layout\\Preload\\";

const auto TEMP_FILE = fs::temp_directory_path() / L"_kbswitch_temp";

std::unordered_map<std::wstring, std::wstring> get_klid_to_text_map() {
  std::unordered_map<std::wstring, std::wstring> map;
  REGSAM permission = KEY_READ | KEY_QUERY_VALUE;

  reg::RegKey key(HKEY_LOCAL_MACHINE, KB_REG_MACHINE, permission);

  for (std::wstring &subkeyName : key.EnumSubKeys()) {
    reg::RegKey subkey(
        HKEY_LOCAL_MACHINE, KB_REG_MACHINE + subkeyName, permission);
    std::wstring layoutText = subkey.GetStringValue(L"Layout Text");
    map.insert(std::make_pair(subkeyName, layoutText));
  }

  KB_LOG(L"map/", L"last error:", GetLastError());
  return map;
}

std::vector<std::wstring> get_user_klid_list() {
  std::vector<std::wstring> res;

  REGSAM permission = KEY_READ | KEY_QUERY_VALUE;

  reg::RegKey key(HKEY_CURRENT_USER, KB_REG_UESR, permission);

  for (auto &p : key.EnumValues()) {
    res.push_back(key.GetStringValue(p.first));
  }

  KB_LOG(L"user list/", L"last error:", GetLastError());

  return res;
}

std::wstring get_current_klid() {
  HWND hwnd = GetForegroundWindow();
  DWORD id = GetWindowThreadProcessId(hwnd, nullptr);
  HKL hkl = GetKeyboardLayout(id);

  // workaround https://stackoverflow.com/questions/64995173
  ActivateKeyboardLayout(hkl, 0);

  std::unique_ptr<wchar_t[]> buffer(new wchar_t[KL_NAMELENGTH]);
  GetKeyboardLayoutNameW(buffer.get());

  KB_LOG(L"current klid/", L"last error:", GetLastError());

  return std::wstring(buffer.get());
}

void set_keyboard_layout(HWND hwnd, const std::wstring &klid) {
  // From MSDN
  // Beginning in Windows 8:
  // The input locale identifier is loaded for the entire system.
  // This function has no effect if the current process does not
  // own the window with keyboard focus.

  // Console program can not get focus, so LoadKeyboardLayout here
  // is used just to get KLID of HKL.

  HKL hkl = LoadKeyboardLayoutW(klid.c_str(), 0);
  KB_LOG(L"set/", L"last error(load):", GetLastError());

  PostMessageW(
      hwnd, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(hkl));
  KB_LOG(L"set/", L"last error(post):", GetLastError());
}

void cache_keyborad_layout_state() {
  KB_LOG(L"cache/", L"temp file:", TEMP_FILE);

  std::wstring current = get_current_klid();
  KB_LOG(L"cache/", L"current:", current);

  // to avoid unnecessary disk writes
  if (fs::exists(TEMP_FILE)) {
    std::wifstream ifs(TEMP_FILE);
    std::wstring cache;
    ifs >> cache;

    KB_LOG(L"cache/", L"read cache:", cache);
    KB_LOG(L"cache/", L"errno:", errno);

    if (cache == current) {
      KB_LOG(L"cache/", L"canceled");
      return;
    }
  }

  std::wofstream ofs(TEMP_FILE);
  ofs << current;

  KB_LOG(L"cache/", L"writed:", current);
  KB_LOG(L"cache/", L"errno:", errno);
}

bool restore_keyboard_layout_state() {
  KB_LOG(L"restore/", L"temp file:", TEMP_FILE);

  if (!fs::exists(TEMP_FILE)) {
    KB_LOG(L"restore/", L"temp file does not exist");
    return false;
  }
  std::wifstream ifs(TEMP_FILE);

  std::wstring buffer;
  ifs >> buffer;

  KB_LOG(L"restore/", L"read:", buffer);
  KB_LOG(L"restore/", L"errno:", errno);

  set_keyboard_layout(GetForegroundWindow(), buffer);

  return true;
}

} // namespace kb
