# kbswitch

a command line utility that helps you switch keyboard layout in Windows, especially made for vim.

## Usage

### Commands

- #### `list`

list all installed keyboard layouts

- #### `get <index>`

get the index of current keyboard layout

- #### `set <index>`

switch to the given keyboard layout

**Note:** `<index>`: The index displayed in `list` command

* ##### Exmaple:
```
0  =>  Chinese (Traditional) - US Keyboard
1  =>  US
2  =>  Japanese
3  =>  Chinese (Simplified) - US Keyboard
```
Then
* `kbswitch.exe set 0`: switch to `Chinese (Traditional) - US Keyboard` keyboard
* `kbswitch.exe set 1`: switch to `US` keyboard
* `kbswitch.exe set 2`: switch to `Janpanese` keyboard
* `kbswitch.exe set 3`: switch to `Chinese (Simplified) - US Keyboard` keyboard

- #### `cache`

save the current keyboard layout state

- #### `restore`

restore the saved state

- #### `help`

show help message

- #### `version`

show the current version


### How to use it in Neovim

1. Make sure kbswitch.exe is in your `PATH`.
2. Add the following lines to your init.lua

**Note**: Don't forget to change the `<index>` of `set` command.

```lua
local kb_switch_group = vim.api.nvim_create_augroup("KbSwitch", { clear = true })

vim.api.nvim_create_autocmd("InsertLeave", {
  pattern = "*",
  group = kb_switch_group,
  callback = function()
    vim.fn.jobstart("kbswitch cache && kbswitch.exe set 1")
  end
})

vim.api.nvim_create_autocmd("InsertEnter", {
  pattern = "*",
  group = kb_switch_group,
  callback = function()
    vim.fn.jobstart("kbswitch restore")
  end
})
```

**Note**: This also works in WSL.

## Permission

- This program need to read from the registry.
- This program need to write to the `%TEMP%` directory.

## Compatibility

I don't really know. But I guess it should at least work in Windows 8+ (I'm using Windows 11 64bit)

## Special Thanks

[GiovanniDicanio/WinReg](https://github.com/GiovanniDicanio/WinReg)

## LICENSE

MIT
