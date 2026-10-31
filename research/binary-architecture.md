# Lightshot Binary Architecture

## Module Decomposition

```
[ Lightshot.exe (Root Launcher) ]
               │
               ▼ Launches versioned instance
[ 5.5.0.7/Lightshot.exe (WTL/ATL Process Host) ]
       │                │                   │
       │ LoadLibrary    │ RegOpenKeyEx      │ Shell_NotifyIconW
       ▼                ▼                   ▼
[ Lightshot.dll ]  [ Windows Registry ]  [ System Tray ]
       │             - Hotkeys            - Context Menu
       │             - Formats            - Balloon Tips
       │             - Proxy
       │
       ├── Desktop Capture:
       │     ├── GDI BitBlt (Primary)
       │     └── DXGIODScreenshot.dll (DirectX 11 Desktop Duplication)
       │
       ├── Selection & UI:
       │     ├── Fullscreen Overlay Window
       │     ├── Selection Rectangle (8 handles)
       │     └── Action Toolbars (GDI+ rendered)
       │
       ├── Annotation:
       │     └── GDI+ Graphics (Pen, Line, Arrow, Rect, Ellipse, Marker, Text, Blur)
       │
       └── Export & Output:
             ├── Clipboard (USER32 OpenClipboard/SetClipboardData)
             ├── Save File (COMDLG32 GetSaveFileNameW)
             └── Upload:
                   ▼ calls UploadImage()
             [ uploader.dll ]
                   │
                   ▼ calls PostExp()
             [ net.dll (libcurl 7.57.0) ]
                   │
                   ▼ HTTPS POST multipart
             [ https://upload.prntscr.com/upload ]
```

## DLL Interfaces

### `Lightshot.dll`
- `void InitLightshot()`
- `void DeinitLightshot()`
- `void MakeScreenshot()`
- `void MakeScreenshotByCommand(int commandId)`
- `void SetTranslations(const wchar_t* langCode)`

### `uploader.dll`
- `int UploadImage(const void* buffer, size_t size, const wchar_t* format, void* callback)`
- `void SetLocaleString(const wchar_t* key, const wchar_t* value)`

### `net.dll`
- `int PostExp(const char* url, void* postData, void* responseBuffer)`
- `void SetCustomProxyExp(int proxyType, const char* proxyHost, int proxyPort)`
- `int UrlReadToString(const char* url, void* responseBuffer)`

### `DXGIODScreenshot.dll`
- `int TakeScreenshotExp(void** outBitmap, int* width, int* height)`

## Clean-Room Linux Mapping

| Windows Component | Original Implementation | Linux Clean-Room Implementation |
|---|---|---|
| Process Host | WTL/ATL exe | Qt 6 `QApplication` |
| System Tray | `Shell_NotifyIconW` | `QSystemTrayIcon` |
| Global Hotkeys | `RegisterHotKey` | X11 XCB key grab + D-Bus GlobalShortcuts |
| Primary Capture | GDI `BitBlt` | XCB SHM (`xcb_shm_get_image`) / Qt Screen Grab |
| Composited Capture | DXGI Desktop Duplication | XDG Desktop Portal (`org.freedesktop.portal.Screenshot`) |
| Selection & Overlay | Win32 layered transparent window | Frameless `QWidget` with custom `paintEvent` |
| Vector Annotation | GDI+ | `QPainter` with antialiasing and composition modes |
| Image Blur / Pixelation | GDI+ custom pixel loop | Direct pixel block averaging on `QImage` |
| Network & Upload | libcurl inside `net.dll` | `QNetworkAccessManager` (asynchronous, HTTPS, proxy-aware) |
| Settings Storage | Windows Registry (`HKCU\Software\...`) | `QSettings` (`~/.config/linuxshot/linuxshot.conf`) |
| Clipboard | Win32 Clipboard | `QGuiApplication::clipboard()` |
| Single Instance | Win32 Named Mutex | `QLockFile` in runtime directory |
