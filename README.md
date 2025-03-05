# kbswitch

A command line utility for Windows that helps you switch keyboard layouts, especially designed for vim users.

## Usage

- ### `--list` or `-l`

Lists all installed keyboard layouts with their index, KLID (Keyboard Layout ID), layout name and active status.

- ### `--current`

Shows the currently active keyboard layout.

- ### `--activate <KLID>` or `-a <KLID>`

Activates the keyboard layout specified by KLID.

* ### Example output of `--list`:
```
 Index        KLID          Layout Name                  Status
 -----        ----          -----------                  ------
   1        00000409        US
   2        00060409        Colemak
   3        00000411        Japanese
   4        00000804        Chinese (Simplified) - US   *Active*
```

Examples:
* `kbswitch --activate 1` or `kbswitch --activate 00000409`: Activate the US keyboard
* `kbswitch --activate 2` or `kbswitch --activate 00060409`: Activate the Colemak keyboard
* `kbswitch --activate 3` or `kbswitch --activate 00000411`: Activate the Japanese keyboard
* `kbswitch --activate 4` or `kbswitch --activate 00000804`: Activate the Chinese (Simplified) keyboard

- ### `--cache` or `-c`

Saves the current keyboard layout to a temporary file for later restoration.

- ### `--restore` or `-r`

Restores the previously cached keyboard layout.

- ### `--version` or `-v`

Shows the current version of kbswitch.

### How to use it in Neovim

1. Make sure `kbswitch.exe` is in your `PATH`.
2. Add the following lines to your `init.lua`:

```lua
local kb_switch_group = vim.api.nvim_create_augroup("KBSwitch", { clear = true })

vim.api.nvim_create_autocmd("InsertLeave", {
  pattern = "*",
  group = kb_switch_group,
  callback = function()
    vim.fn.jobstart("kbswitch --cache && kbswitch --activate 00000409") -- Replace with your preferred layout KLID
  end
})

vim.api.nvim_create_autocmd("InsertEnter", {
  pattern = "*",
  group = kb_switch_group,
  callback = function()
    vim.fn.jobstart("kbswitch --restore")
  end
})
```

**Note**: This setup allows Neovim to automatically:
1. Cache your current keyboard layout and switch to a specified layout when leaving insert mode
2. Restore your previous layout when entering insert mode

## Requirements and Permissions

- This program requires read access to the Windows registry to query keyboard layouts.
- This program writes to the `%TEMP%` directory to store cached keyboard layout information.

## Compatibility

Compatible with modern Windows systems (Windows 8+). Tested on Windows 11 64-bit.

## LICENSE

MIT
