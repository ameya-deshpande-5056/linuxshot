# LinuxShot Feature Matrix

Based on reverse-engineering analysis of Lightshot 5.5.0.7 locale files, binary imports, and module architecture.

## Application Architecture (Observed)

| Component | Binary | Purpose | Linux Equivalent |
|-----------|--------|---------|-----------------|
| Launcher | `app/Lightshot.exe` (227KB) | Updater/launcher stub | Direct launch |
| Main App | `app/5.5.0.7/Lightshot.exe` (500KB) | App host, tray, hotkeys, lifecycle | Qt6 app core |
| Screenshot Engine | `app/5.5.0.7/Lightshot.dll` (503KB) | Capture, selection, annotation, UI | Core module |
| DXGI Capture | `app/5.5.0.7/DXGIODScreenshot.dll` (96KB) | Desktop Duplication API capture | X11/XCB + Portal |
| Networking | `app/5.5.0.7/net.dll` (533KB) | HTTP/HTTPS, TLS (libcurl-like) | Qt Network |
| Uploader | `app/5.5.0.7/uploader.dll` (221KB) | Image upload to prntscr.com | Pluggable upload |
| Locales | `app/5.5.0.7/locales/*.txt` | 50+ languages | Qt i18n |

## Core Features

| Feature | Observed | Documented | Implemented | Tested | Status |
|---------|----------|------------|-------------|--------|--------|
| **Capture** | | | | | |
| Region capture | ✅ | ✅ | ☐ | ☐ | Pending |
| Full screen capture | ✅ | ✅ | ☐ | ☐ | Pending |
| Instant save fullscreen | ✅ | ✅ | ☐ | ☐ | Pending |
| Instant upload fullscreen | ✅ | ✅ | ☐ | ☐ | Pending |
| Cursor capture option | ✅ | ✅ | ☐ | ☐ | Pending |
| Keep selection position | ✅ | ✅ | ☐ | ☐ | Pending |
| Multi-monitor support | ✅ (DXGI) | ✅ | ☐ | ☐ | Pending |
| **Selection** | | | | | |
| Drag to select region | ✅ | ✅ | ☐ | ☐ | Pending |
| Resize selection | ✅ | ✅ | ☐ | ☐ | Pending |
| Move selection | ✅ | ✅ | ☐ | ☐ | Pending |
| Select full screen | ✅ | ✅ | ☐ | ☐ | Pending |
| Clear selection | ✅ | ✅ | ☐ | ☐ | Pending |
| Cancel selection (Escape) | ✅ | ✅ | ☐ | ☐ | Pending |
| **Annotation Tools** | | | | | |
| Pen (freehand) | ✅ | ✅ | ☐ | ☐ | Pending |
| Line | ✅ | ✅ | ☐ | ☐ | Pending |
| Arrow | ✅ | ✅ | ☐ | ☐ | Pending |
| Rectangle | ✅ | ✅ | ☐ | ☐ | Pending |
| Circle | ✅ | ✅ | ☐ | ☐ | Pending |
| Marker (highlighter) | ✅ | ✅ | ☐ | ☐ | Pending |
| Text | ✅ | ✅ | ☐ | ☐ | Pending |
| Blur/Pixelation | ✅ | ✅ | ☐ | ☐ | Pending |
| Color picker | ✅ | ✅ | ☐ | ☐ | Pending |
| Undo | ✅ | ✅ | ☐ | ☐ | Pending |
| **File Operations** | | | | | |
| Save | ✅ | ✅ | ☐ | ☐ | Pending |
| Copy to clipboard | ✅ | ✅ | ☐ | ☐ | Pending |
| Print | ✅ | ✅ | ☐ | ☐ | Pending |
| Upload to server | ✅ | ✅ | ☐ | ☐ | Pending |
| Open file | ✅ | ✅ | ☐ | ☐ | Pending |
| Edit online | ✅ | ✅ | ☐ | ☐ | Pending |
| **Upload** | | | | | |
| Upload to prntscr.com | ✅ | ✅ | ☐ (own server) | ☐ | Pending |
| Auto-copy link after upload | ✅ | ✅ | ☐ | ☐ | Pending |
| Auto-close upload window | ✅ | ✅ | ☐ | ☐ | Pending |
| Upload progress | ✅ | ✅ | ☐ | ☐ | Pending |
| Upload retry | ✅ | ✅ | ☐ | ☐ | Pending |
| **Sharing** | | | | | |
| Google image search | ✅ | ✅ | ☐ | ☐ | Pending |
| TinEye search | ✅ | ✅ | ☐ | ☐ | Pending |
| Email | ✅ | ✅ | ☐ | ☐ | Pending |
| Twitter/Facebook/VK/Pinterest | ✅ | ✅ | ☐ | ☐ | Pending |
| **Settings** | | | | | |
| General hotkey | ✅ | ✅ | ☐ | ☐ | Pending |
| Save fullscreen hotkey | ✅ | ✅ | ☐ | ☐ | Pending |
| Upload fullscreen hotkey | ✅ | ✅ | ☐ | ☐ | Pending |
| Language selection | ✅ | ✅ | ☐ | ☐ | Pending |
| Proxy settings | ✅ | ✅ | ☐ | ☐ | Pending |
| Upload format (PNG/JPEG) | ✅ | ✅ | ☐ | ☐ | Pending |
| JPEG quality | ✅ | ✅ | ☐ | ☐ | Pending |
| Show notifications | ✅ | ✅ | ☐ | ☐ | Pending |
| PNG optimization (OptiPNG) | ✅ | ✅ | ☐ | ☐ | Pending |
| **System Integration** | | | | | |
| System tray | ✅ | ✅ | ☐ | ☐ | Pending |
| Global hotkeys | ✅ | ✅ | ☐ | ☐ | Pending |
| Notifications | ✅ | ✅ | ☐ | ☐ | Pending |
| Auto-start | ✅ | ✅ | ☐ | ☐ | Pending |
| Single instance | ✅ | ✅ | ☐ | ☐ | Pending |
| **Account** | | | | | |
| Sign in | ✅ | ✅ | ☐ | ☐ | Pending |
| Gallery | ✅ | ✅ | ☐ | ☐ | Pending |
| Sign out | ✅ | ✅ | ☐ | ☐ | Pending |

## API Dependencies (Windows → Linux Mapping)

| Windows API | Lightshot Usage | Linux Equivalent |
|-------------|-----------------|-----------------|
| GDI (BitBlt, CreateDC) | Screen capture | XCB/X11 SHM |
| DXGI Desktop Duplication | GPU screen capture | XDG Portal + PipeWire |
| GDI+ | Drawing, annotation | QPainter / Qt Quick Canvas |
| RegisterHotKey | Global shortcuts | XCB key grabs / D-Bus GlobalShortcuts |
| Shell_NotifyIcon | System tray | QSystemTrayIcon |
| OpenClipboard/SetClipboardData | Clipboard | QClipboard |
| GetSaveFileName | Save dialog | QFileDialog |
| WinINet (HttpSendRequest) | HTTP upload | QNetworkAccessManager |
| CRYPT32 | TLS/certificates | Qt TLS backend |
| Registry | Settings storage | QSettings (INI/XDG) |
| CreateMutex | Single instance | QLockFile / D-Bus |
| EnumDisplayMonitors | Multi-monitor | QScreen / XRandR |

## Toolbar Layout (Confirmed from locale)

**Left toolbar (annotation tools):**
1. Pen
2. Line  
3. Arrow
4. Rectangle
5. Circle
6. Marker
7. Text
8. Blur
9. Color picker
10. Undo

**Right toolbar (actions):**
1. Save
2. Copy
3. Print
4. Upload
5. Share (submenu)
6. Edit online
7. Select full screen
8. Close/Cancel

## Keyboard Shortcuts (Confirmed)

| Action | Shortcut |
|--------|----------|
| General capture | Print Screen (configurable) |
| Instant save fullscreen | Configurable |
| Instant upload fullscreen | Configurable |
| Cancel | Escape |

## Settings Categories

1. **General** — Language, notifications, cursor capture, keep selection
2. **Hotkeys** — General, save fullscreen, upload fullscreen
3. **Formats** — Upload format (PNG/JPEG), JPEG quality, optimization
4. **Proxy** — No proxy, system proxy, manual proxy (HTTP/HTTPS + port)

