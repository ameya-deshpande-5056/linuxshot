# Lightshot Static Analysis — Strings & Feature Mapping

> **Purpose**: Document observable strings, configuration keys, UI elements, URLs, and feature names
> from Lightshot binaries (v5.5.0.4, built 2019-07-22) for clean-room reimplementation guidance.
>
> **Method**: `strings -e l` (wide/UTF-16LE) and `strings` (ASCII) on PE binaries.
> **Legal**: No proprietary code reproduced. Observable behavior and API patterns only.

---

## 1. UI Feature Strings (Lightshot.dll)

### 1.1 Annotation Tool Labels

| String | Localization Key | Tool |
|--------|-----------------|------|
| `Select area` | (no key) | Initial mode |
| `Line` | `[[screenshot_plugin.edit_line]]` | Freehand pencil |
| `Arrow` | `[[screenshot_plugin.edit_arrow]]` | Arrow annotation |
| `Rect` | `[[screenshot_plugin.edit_rect]]` | Rectangle shape |
| `Marker` | `[[screenshot_plugin.edit_marker]]` | Highlighter marker |
| `Text` | `[[screenshot_plugin.edit_text]]` | Text overlay |
| `Color` | `[[screenshot_plugin.edit_color]]` | Color picker |
| `Undo` | `[[screenshot_plugin.edit_undo]]` | Undo (Ctrl+Z) |

**Confidence**: Confirmed — exact strings from Lightshot.dll.

### 1.2 Action Button Labels & Keyboard Shortcuts

| String | Shortcut | Localization Key |
|--------|----------|-----------------|
| `Upload` | `Ctrl+D` | `[[screenshot_plugin.upload]]` |
| `Print` | `Ctrl+P` | `[[screenshot_plugin.print]]` |
| `Copy` | `Ctrl+C` | `[[screenshot_plugin.copy]]` |
| `Save` | `Ctrl+S` | `[[screenshot_plugin.save]]` |
| `Fullscreen` | `Ctrl+A` | `[[screenshot_plugin.fullscreen]]` |
| `Clear selection` | — | `[[screenshot_plugin.clear]]` |
| `Cancel` | `Esc` | `[[screenshot_plugin.cancel]]` |
| `Close` | `Ctrl+X` | `[[screenshot_plugin.close]]` |

### 1.3 Social Sharing

| String | Localization Key |
|--------|-----------------|
| `Share on Social Networks` | `[[screenshot_plugin.share]]` |
| `Search for similar images on Google` | `[[screenshot_plugin.share_googlesearch]]` |
| `Share on Twitter` | `[[screenshot_plugin.share_twitter]]` |
| `Share on Facebook` | `[[screenshot_plugin.share_facebook]]` |
| `Share on VK` | `[[screenshot_plugin.share_vk]]` |
| `Share on Pinterest` | `[[screenshot_plugin.share_pinterest]]` |

### 1.4 Status/Notification Messages

| String | Localization Key |
|--------|-----------------|
| `Screenshot copied to clipboard` | `[[screenshot_plugin.screenshot_copied_to_clipboard]]` |
| `Screenshot saved to %filename%. Click here to open in folder.` | `[[screenshot_plugin.screenshot_saved]]` |
| `Screenshot uploaded. Link copied to your clipboard.` | `[[screenshot_plugin.screenshot_uploaded_link_copied]]` |
| `Uploading Image` | `[[screenshot_plugin.uploading_window_capt]]` |
| `Uploading failed, retry?` | `[[screenshot_plugin.upload_failed_retry]]` |
| `Uploading failed.` | `[[screenshot_plugin.upload_failed]]` |

### 1.5 Dimension Display

- Format string: `%dx%d` — confirms pixel dimensions shown during selection.

---

## 2. UI Strings (Lightshot.exe — Main App)

### 2.1 Tray Menu Items

| String | Localization Key |
|--------|-----------------|
| `Take screenshot` | `[[screenshot_app.take_screenshot]]` |
| `Options` | `[[screenshot_app.options]]` |
| `Help` | `[[screenshot_app.help]]` |
| `Exit` | `[[screenshot_app.exit]]` |
| `Sign Out` | `[[screenshot_app.menu.sign_out]]` |
| `My gallery (%username%)` | `[[screenshot_app.menu.gallery]]` |
| `Sign In...` | `[[screenshot_app.menu.sign_in]]` |
| `About` | `[[screenshot_app.about]]` |
| `Terms of Service` | `[[screenshot_app.terms]]` |

### 2.2 Options Dialog — Tabs & Settings

**General Tab** (`[[screenshot_app.options.general]]`):
| Setting Label | Registry Key | Localization Key |
|--------------|-------------|-----------------|
| `Automatically copy link after upload` | `AutoCopy` | `[[screenshot_app.options.autocopy]]` |
| `Automatically close upload window` | `AutoClose` | `[[screenshot_app.options.autoclose]]` |
| `Show notification bubbles on copy and save` | `ShowBubbles` | `[[screenshot_app.options.showbubbles]]` |
| `Keep selected area position` | `KeepSelection` | `[[screenshot_app.options.keepselection]]` |
| `Capture cursor` | `CaptureCursor` | `[[screenshot_app.options.capturecursor]]` |
| `Language` | — | `[[screenshot_app.options.language]]` |

**Formats Tab** (`[[screenshot_app.options.formats]]`):
| Setting Label | Registry Key | Localization Key |
|--------------|-------------|-----------------|
| `Upload images format` | `UploadFormat` | `[[screenshot_app.options.uploadformat]]` |
| `JPEG quality` | `JpegQuality` | `[[screenshot_app.options.jpegquality]]` |

**Hotkeys Tab** (`[[screenshot_app.options.hotkeys]]`):
| Setting Label | Registry Keys | Localization Key |
|--------------|--------------|-----------------|
| `General hotkey` | `Hotkey_main_enabled`, `Hotkey_main_mod`, `Hotkey_main_vk` | `[[screenshot_app.options.hotkeygeneral]]` |
| `Instant save fullscreen` | `Hotkey_savefull_enabled`, `Hotkey_savefull_mod`, `Hotkey_savefull_vk` | `[[screenshot_app.options.hotkeysavefullscreen]]` |
| `Instant upload fullscreen` | `Hotkey_uploadfull_enabled`, `Hotkey_uploadfull_mod`, `Hotkey_uploadfull_vk` | `[[screenshot_app.options.hotkeyuploadfullscreen]]` |

Modifier key display strings: `Ctrl + `, `Shift + `, `Alt + `, `Win + `

**Proxy Tab** (`[[screenshot_app.options.proxy]]`):
| Setting Label | Localization Key |
|--------------|-----------------|
| `No proxy` | `[[screenshot_app.options.noproxy]]` |
| `Use system proxy settings` | `[[screenshot_app.options.systemproxy]]` |
| `Manual proxy configuration` | `[[screenshot_app.options.manualproxy]]` |
| `HTTP(S) proxy` | `[[screenshot_app.options.httpsproxy]]` |
| `Port` | `[[screenshot_app.options.proxyport]]` |

### 2.3 Tray Tooltip

- `Press the "%hotkey_main%" on your keyboard to take a screenshot`

### 2.4 Hotkey Error Messages

- `Failed to register the hotkey:`
- `One of your hotkeys is invalid...`
- `You can not set the same hotkeys for different functions...`

---

## 3. Registry Keys & Configuration

### 3.1 Registry Path
**Confirmed**: `Software\SkillBrains\Lightshot` (under `HKEY_CURRENT_USER`)

### 3.2 All Identified Registry Value Names

| Key | Purpose | Source Binary |
|-----|---------|--------------|
| `AutoCopy` | Auto-copy upload link | uploader.dll, Lightshot.exe |
| `AutoClose` | Auto-close upload dialog | uploader.dll, Lightshot.exe |
| `ShowBubbles` | Show notification balloons | Lightshot.dll, Lightshot.exe |
| `KeepSelection` | Persist selection region | Lightshot.dll, Lightshot.exe |
| `CaptureCursor` | Include cursor in capture | Lightshot.dll, Lightshot.exe |
| `JpegQuality` | JPEG quality (slider value) | Lightshot.dll, Lightshot.exe |
| `UploadFormat` | Format for uploads (JPEG/PNG) | Lightshot.dll, Lightshot.exe |
| `Format` | Save format preference | Lightshot.dll |
| `LastSavedDir` | Last save directory | Lightshot.dll |
| `Hotkey_main_enabled` | Main hotkey on/off | Lightshot.exe |
| `Hotkey_main_mod` | Main hotkey modifiers | Lightshot.exe |
| `Hotkey_main_vk` | Main hotkey virtual key | Lightshot.exe |
| `Hotkey_savefull_enabled` | Save fullscreen hotkey on/off | Lightshot.exe |
| `Hotkey_savefull_mod` | Save fullscreen modifiers | Lightshot.exe |
| `Hotkey_savefull_vk` | Save fullscreen virtual key | Lightshot.exe |
| `Hotkey_uploadfull_enabled` | Upload fullscreen hotkey on/off | Lightshot.exe |
| `Hotkey_uploadfull_mod` | Upload fullscreen modifiers | Lightshot.exe |
| `Hotkey_uploadfull_vk` | Upload fullscreen virtual key | Lightshot.exe |
| `ProxyType` | Proxy mode (none/system/manual) | uploader.dll |
| `ProxyString` | Manual proxy string | uploader.dll |
| `appFirstRun` | First-run flag | Lightshot.exe |

### 3.3 Machine Identification

- Reads `SOFTWARE\Microsoft\Cryptography\MachineGuid` for unique device ID
- `appId` stored/generated for API auth

---

## 4. File Formats & Save Patterns

### 4.1 Supported Image Formats

| Extension | MIME Type | Filter String |
|-----------|----------|---------------|
| `.png` | `image/png` | `*.png` |
| `.jpg` / `.jpeg` | `image/jpeg` | `*.jpg` |
| `.bmp` | `image/bmp` | `*.bmp` |

### 4.2 Additional Print/Export Formats

| Extension | Description |
|-----------|-------------|
| `.xps` | XPS Document (`*.xps`) — via Microsoft XPS Document Writer |
| `.pdf` | PDF Document (`*.pdf`) — via Adobe PDF printer |

### 4.3 File Naming

- Default: `Screenshot_1`, incrementing with `Screenshot_%d`
- Thumbnail suffix: `_thumb.jpg`

---

## 5. URLs & Network Endpoints

### 5.1 Upload Endpoint (from uploader.dll)

| URL | Purpose | Confidence |
|-----|---------|------------|
| `https://upload.prntscr.com/upload` | Image upload endpoint | **Confirmed** |

Upload parameters (multipart POST):
- `image` — the screenshot binary
- `thumb` — thumbnail binary
- `app_token` — authentication token
- `app_id` — application ID
- Response fields: `status`, `success`, `direct_link`, `width`, `height`

### 5.2 API Endpoint (from Lightshot.exe)

| URL | Purpose | Confidence |
|-----|---------|------------|
| `https://api.prntscr.com/v1.1/` | JSON-RPC API (auth, gallery) | **Confirmed** |
| `https://prntscr.com/` | Website base | **Confirmed** |
| `https://app.prntscr.com/` | App-specific web features | **Confirmed** |
| `app/attach_app.php?id=` | App attachment endpoint | **Confirmed** |

### 5.3 JSON-RPC Methods (from Lightshot.exe)

- `attach_application` — link app to user account
- `detach_application` — unlink app
- `get_user` — retrieve user info
- Fields: `jsonrpc`, `method`, `params`, `result`, `success`, `username`, `token`, `app_id`, `login_token`, `app_description`, `app_type`

### 5.4 Social Sharing URLs

| Platform | URL Pattern |
|----------|------------|
| Google Search by Image | `https://www.google.com/searchbyimage?image_url=` |
| Twitter | `https://twitter.com/home?source=Lightshot&status=` |
| Facebook | `https://www.facebook.com/sharer.php?u=` |
| VKontakte | `https://vk.com/share.php?url=` |
| Pinterest | `https://pinterest.com/pin/create/button/?url=...&media=` |

### 5.5 Analytics

- Google Analytics beacon: `http://www.google-analytics.com/__utm.gif?utmwv=4.4sh`
- Tracking ID: `UA-31173726-1`
- Tracked values: `ga_unique_id`, `ga_first_time`, `ga_counter`, `ga_last_time`

---

## 6. Localization System

### 6.1 Pattern

All UI strings use the pattern: `[[namespace.key]]`

Namespaces discovered:
- `screenshot_plugin.*` — annotation tools, actions, status messages (in Lightshot.dll)
- `screenshot_app.*` — tray menu, options, sign-in (in Lightshot.exe)

### 6.2 Translation Mechanism

- Lightshot.dll exports `SetTranslations` — translations are passed in from Lightshot.exe
- Supported locale codes: 100+ entries including `en-US`, `ru-RU`, `de-DE`, `fr-FR`, `ja-JP`, etc.

---

## 7. Mutex / Window Class Names

| Name | Purpose | Confidence |
|------|---------|------------|
| `COOL_SCREENSHOT_MUTEX_YARRR` | Single-instance mutex for screenshot DLL | **Confirmed** |
| `LightshotStandAloneAppMainMutex` | Single-instance for main app | **Confirmed** |
| `Lightshot_Tray_Wnd` | Tray notification window class | **Confirmed** |
| `ATL:%p` | ATL window class format | **Confirmed** |

---

## 8. Font Names

| Font | Usage |
|------|-------|
| `Calibri` | Annotation text | 
| `Arial` | Fallback/alternative |
| `MS Shell Dlg` | Dialog UI |

---

## 9. Upload Engine Selection (uploader.dll)

Strings indicate dual-engine support:
- `WinInet` — Windows Internet API engine
- `Curl` — libcurl engine
- `EngineStats` — tracks per-engine success/failure
- `total_upload_fail`, `fail_upload_`, `success_upload_` — statistics tracking

Upload format negotiation:
- `application/octet-stream` — binary upload content type
- `True` — literal string in upload params

---

## 10. Build Information

| Binary | Internal Name | Version | PDB Path Fragment |
|--------|-------------|---------|-------------------|
| Lightshot.dll | LightshotDll | 5.5.0.4 | — |
| uploader.dll | Uploader | 5.5.0.4 | `D:\Dev\skillbrains\lightshot-windows-app\Screenshot\StandAloneApp\Release\curl_uploader.pdb` |
| net.dll | NetDll | 5.5.0.4 | — |
| DXGIODScreenshot.dll | DXGIODScreenshot | 5.5.0.4 | `D:\Dev\skillbrains\lightshot-windows-app\Screenshot\StandAloneApp\Release\DXGIODScreenshot.pdb` |
| Lightshot.exe | Lightshot | 5.5.0.4 | — |
| Launcher (Lightshot.exe) | Starter | 1.0.0.1 | (Original: `Starter.exe`) |

**Company**: Skillbrains  
**Copyright**: 2009-2019  
**Build Date**: Mon Jul 22 11:15-11:16 2019 (all DLLs built within 1 minute)  
**Build Environment**: Visual Studio 2015 (Linker 14.0), PE32 (32-bit x86)

---

## 11. C++ Class Names (from RTTI strings)

| Class Name | Binary | Purpose |
|------------|--------|---------|
| `CUserRect` | Lightshot.dll | Rectangle annotation object |
| `CUserText` | Lightshot.dll | Text annotation object |
| `CImageUploaderSync` | uploader.dll | Image upload handler |
| `CPrntscrUploaderSync` | uploader.dll | Prntscr-specific upload handler |
| `CSigninProgressDialog` | Lightshot.exe | Sign-in progress UI |
| `CFormatsPage` | Lightshot.exe | Formats options page |
| `CHotkeysPage` | Lightshot.exe | Hotkeys options page |
| `CProxyPage` | Lightshot.exe | Proxy options page |
| `CCustomHotkeyControl` | Lightshot.exe | Custom hotkey input control |
| `CRequestAttach` / `CRequestAttachReceiver` | Lightshot.exe | API attach request |
| `CRequestDetach` / `CRequestDetachReceiver` | Lightshot.exe | API detach request |
| `CRequestGetUser` / `CRequestGetUserReceiver` | Lightshot.exe | API get user request |

---

## 12. DPI Awareness

From the embedded manifest in Lightshot.exe:
```xml
<dpiAware>True/PM</dpiAware>
```
This indicates per-monitor DPI awareness. The `Shcore.dll` reference in Lightshot.dll confirms use of `SetProcessDpiAwareness` or similar APIs.

