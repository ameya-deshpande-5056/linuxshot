# Lightshot Installer Analysis

## Summary

- **Source File**: `setup-lightshot.exe`
- **File Size**: 2,786,328 bytes (2.66 MiB)
- **MD5**: `a1f6923e771b4ff0df9fec9555f97c65`
- **SHA256**: `928c2808421dfd487ffa697379548cbe682c0e13aeb595eb89973ba9c515b8a1`
- **Installer Framework**: Inno Setup
- **Setup Data Version**: 5.6.0 (Unicode)
- **Target Application**: Lightshot 5.5.0.7
- **Target OS / Architecture**: Windows 32-bit (x86 / Intel 80386)

## Extraction Method

Extracted cleanly using `innoextract` version 1.9 on Linux without executing any Windows binaries:

```bash
innoextract -d research/extracted setup-lightshot.exe
```

## Embedded Components

The installer packages two logical sets of files:

### 1. Temporary Installer Assets (`tmp/`)
- `downloader.exe` (187,712 bytes): Auxiliary downloader for optional bundled offerings.
- `setupupdater.exe` (886,032 bytes): Updater component.
- Bundled third-party promotional assets (Yandex browser RTF EULA, bitmaps).

### 2. Application Files (`app/`)
- `app/Lightshot.exe` (226,728 bytes): Top-level launcher stub in `%ProgramFiles(x86)%\Skillbrains\Lightshot`.
- `app/5.5.0.7/`: Version-isolated application payload:
  - `Lightshot.exe` (499,624 bytes): Main application process (system tray, hotkeys, options dialog).
  - `Lightshot.dll` (502,696 bytes): Core screenshot engine (desktop capture, selection overlay, GDI+ annotation).
  - `uploader.dll` (220,584 bytes): Upload orchestration, social media share URLs, clipboard sync.
  - `net.dll` (532,904 bytes): libcurl-based HTTP/HTTPS networking and proxy handling.
  - `DXGIODScreenshot.dll` (95,656 bytes): DirectX 11 / DXGI Desktop Duplication API capture backend.
  - `locales/*.txt` (50 localized string files): Full string tables for 50 languages.
  - `learnmore.url`, `learnmore_ru.url`: Shortcut links to documentation.

## Registry & System Integration

From static string and WTL RTTI analysis:
- **Registry Root**: `HKCU\Software\SkillBrains\Lightshot`
- **Startup Integration**: Optional `Run` key entry in `HKCU\Software\Microsoft\Windows\CurrentVersion\Run`
- **Installation Directory**: `%ProgramFiles(x86)%\Skillbrains\Lightshot\`
