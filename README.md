﻿# kbswitch

> a simple utility that help you switch keyboard layout in Windows.

## Usage

### Commands

- #### `list`
list all installed keyboard layouts

* ##### Exmaple:

```
$ kbswitch.exe list
0       US
1       Japanese
2       Chinese (Simplified) - US Keyboard
```

- #### `set <number>`

switch to the given keyboard layout  
`<number>`: The number displayed in `list` command

- #### `cache`

save the current keyboard layout state

- #### `restore`

restore to the saved state

- #### `help`

show help message


### For Vim / NeoVim / Vim Emulator users

1. Make sure kbswitch.exe is in your `PATH`.
2. Add the following lines to your (Neo)Vim / vim emulator 's configuation file.  
**Note**: Don't forget to change the `<number>` of `set` command.

#### (Neo)Vim

```vim
augroup KeyboardLayoutSwitch
    autocmd!
    autocmd InsertLeave * :silent !kbswitch.exe restore
    autocmd InsertEnter * :silent !kbswitch.exe cache && kbswitch.exe set 0
augroup END
```

**Note**: This should also works in WSL.

#### Vim Emulators (VSCodeVim, VsVim etc.)

As most of the vim emulators do not supprot `autocmd`, we have to use key map here.
```vim
inoremap <Esc> <Esc>:!kbswitch.exe cache && kbswitch.exe set 0<CR>

nnoremap i :!kbswitch.exe restore<CR>i
nnoremap I :!kbswitch.exe restore<CR>I
nnoremap o :!kbswitch.exe restore<CR>o
nnoremap O :!kbswitch.exe restore<CR>O
nnoremap a :!kbswitch.exe restore<CR>a
nnoremap A :!kbswitch.exe restore<CR>A
nnoremap cc :!kbswitch.exe restore<CR>cc
nnoremap C :!kbswitch.exe restore<CR>C
nnoremap gi :!kbswitch.exe restore<CR>gi
nnoremap gI :!kbswitch.exe restore<CR>gI
```

## Permission

- This program need to read from the registry.
- This program need to wirte to the `%TEMP%` directory.

## Compatibility

I don't really know. But it just works on Windows 11 (64bit).

## Special Thanks

[GiovanniDicanio/WinReg](https://github.com/GiovanniDicanio/WinReg)

## LICENSE
MIT