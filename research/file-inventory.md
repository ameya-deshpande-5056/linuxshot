# Extracted File Inventory

Inventory of all files extracted from `setup-lightshot.exe` (Lightshot v5.5.0.7).

| Relative Path | Size (Bytes) | SHA-256 Hash | File Classification | Purpose / Role | Requires Further Analysis |
|---|---|---|---|---|---|
| `app/Lightshot.exe` | 226,728 | `8d4c4bcf7d7aedf0480e3eaac52138e63724ae83c419de8a98d6ab32d1c93645` | PE32 Executable (GUI, x86) | Root launcher stub, launches versioned executable | No (simple stub) |
| `app/5.5.0.7/Lightshot.exe` | 499,624 | `0b899508777d7ed5159e2a99a5eff60c54d0724493df3d630525b837fa43aa51` | PE32 Executable (GUI, x86) | Main application host: Tray icon, global hotkeys, settings dialog (WTL/ATL) | Yes (Analyzed) |
| `app/5.5.0.7/Lightshot.dll` | 502,696 | `04a27f0d1e89341722461119e00a10e00ec2a52f5e305961161ec4378e610e93` | PE32 DLL (x86) | Screenshot engine: Capture, selection window, GDI+ annotation tools, export | Yes (Analyzed) |
| `app/5.5.0.7/uploader.dll` | 220,584 | `037ee2f3243918fffa71b9e3fe0541245f75f89abcac0ccf2ea6a57020ddaad7` | PE32 DLL (x86) | Upload management: Multipart upload, social share URLs, response parsing | Yes (Analyzed) |
| `app/5.5.0.7/net.dll` | 532,904 | `45b2c27a4345d789287539dd82c9f85ac9324d01825f6e2e0c2cdd4c4172c038` | PE32 DLL (x86) | Network layer: Embedded libcurl 7.57.0 with TLS and proxy support | Yes (Analyzed) |
| `app/5.5.0.7/DXGIODScreenshot.dll` | 95,656 | `6acdcd817cc5df637aa4cd101c25c9e0a69c778347a7a40ce7511eeea26fd6f0` | PE32 DLL (x86) | DXGI / D3D11 Desktop Duplication API capture provider | Yes (Analyzed) |
| `app/5.5.0.7/learnmore.url` | 60 | `c8345f016b914c6502173cd41bf7cd23d6be3fd6f7d8f274845fe02595538b37` | Internet Shortcut | Documentation link (http://app.prntscr.com/learnmore.html) | No |
| `app/5.5.0.7/learnmore_ru.url` | 63 | `78c231231ac2c07ae87a1e3bec5869d6568a16c66e5922af1310b811837f8925` | Internet Shortcut | Documentation link (RU) | No |
| `app/5.5.0.7/locales/en.txt` | 10,420 | `4a57246bd4ce9d387ec10f0ab2084c3d91e8463d03c1412f3665aee3885a85a5` | UTF-8 text (with BOM) | English localization strings & feature declarations | Yes (Analyzed) |
| `app/5.5.0.7/locales/*.txt` (49 others) | Various | Various | UTF-8 text (with BOM) | 49 additional language localization tables | No |
| `tmp/downloader.exe` | 187,712 | `8e553e9aa721db167bdeaf7748bb09d4f497e3a469fd09b6a995ea25d378f1fb` | PE32 Executable (x86) | Optional installer partner downloader | No |
| `tmp/setupupdater.exe` | 886,032 | `088f048ee972ef80bd527e301431c1ad7e46d0c994ad8a2b586c4fa6d86ac399` | PE32 Executable (x86) | Self-updater installer payload | No |
| `tmp/*.bmp`, `tmp/*.rtf` | Various | Various | Bitmaps and RTF | Promotional partner installation dialogs | No |
