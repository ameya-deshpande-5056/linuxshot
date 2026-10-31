# Settings and Configuration Inventory

Complete mapping of Lightshot settings keys to LinuxShot XDG configuration.

## File Location
- **Original Windows**: `HKCU\Software\SkillBrains\Lightshot`
- **LinuxShot (Clean-Room)**: `~/.config/linuxshot/linuxshot.conf` (QSettings INI format conforming to XDG Base Directory specification).

## Keys & Values

| Windows Registry Key | LinuxShot INI Key | Type | Default Value | Description |
|---|---|---|---|---|
| `Hotkey_main_enabled` | `hotkeys/main_enabled` | Boolean | `true` | Enable general capture shortcut |
| `Hotkey_main_vk` + `_mod` | `hotkeys/main` | String | `Print` | Key combination for general capture |
| `Hotkey_savefull_enabled` | `hotkeys/savefull_enabled` | Boolean | `true` | Enable instant save shortcut |
| `Hotkey_savefull_vk` + `_mod`| `hotkeys/savefull` | String | `Shift+Print`| Key combination for instant save |
| `Hotkey_uploadfull_enabled` | `hotkeys/uploadfull_enabled` | Boolean | `false` | Enable instant upload shortcut |
| `Hotkey_uploadfull_vk` + `_mod` | `hotkeys/uploadfull` | String | `Ctrl+Shift+Print` | Key combination for instant upload |
| `language` | `general/language` | String | `en` | UI Language |
| `showbubbles` | `general/show_notifications` | Boolean | `true` | Display desktop notifications on save/copy |
| `capturecursor` | `general/capture_cursor` | Boolean | `false` | Include mouse cursor in capture |
| `keepselection` | `general/keep_selection` | Boolean | `false` | Retain prior selection bounds on next capture |
| `autocopy` | `upload/auto_copy_link` | Boolean | `true` | Automatically copy resulting URL to clipboard |
| `autoclose` | `upload/auto_close_window` | Boolean | `true` | Close upload progress window upon success |
| `UploadFormat` | `formats/upload_format` | String | `png` | Image upload encoding (`png` or `jpeg`) |
| `JpegQuality` | `formats/jpeg_quality` | Integer | `90` | Compression quality for JPEG (1-100) |
| *(None)* | `formats/default_save_dir` | String | `~/Pictures` | Default directory for saving screenshots |
| `ProxyType` | `network/proxy_type` | Integer | `0` | Proxy mode (0=none, 1=system, 2=manual) |
| `ProxyString` | `network/proxy_host` | String | `""` | Manual proxy hostname / IP |
| *(Port)* | `network/proxy_port` | Integer | `8080` | Manual proxy port |
| *(None)* | `upload/provider_url` | String | `""` | Custom upload endpoint URL |
| *(None)* | `annotation/last_color` | String | `#FF0000` | Last selected annotation color (hex) |
| *(None)* | `annotation/stroke_width` | Integer | `3` | Default annotation pen stroke width (px) |
