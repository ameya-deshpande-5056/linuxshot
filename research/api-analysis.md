# Lightshot API & Import Analysis

> **Purpose**: Document Windows API usage patterns from PE import tables to guide
> Linux reimplementation with equivalent APIs.
>
> **Method**: `objdump -x` / `objdump -p` on PE binaries.
> **Legal**: Only publicly documented Windows API names are listed. No proprietary code.

---

## 1. Lightshot.dll — Core Screenshot & Annotation Engine

### 1.1 GDI+ (gdiplus.dll) — 67 imported functions

**Bitmap Creation & Conversion:**
| Function | Linux Equivalent | Purpose |
|----------|-----------------|---------|
| `GdipCreateBitmapFromScan0` | Cairo `cairo_image_surface_create_for_data` | Create bitmap from raw pixel data |
| `GdipCreateBitmapFromHBITMAP` | — | Convert GDI HBITMAP to GDI+ Bitmap |
| `GdipCreateBitmapFromStream` | — | Load bitmap from stream |
| `GdipCreateHBITMAPFromBitmap` | — | Convert GDI+ Bitmap back to HBITMAP |
| `GdipCloneImage` | `cairo_surface_create_similar` | Duplicate an image |
| `GdipDisposeImage` | `cairo_surface_destroy` | Free image |
| `GdipGetImageWidth` / `GdipGetImageHeight` | `cairo_image_surface_get_width/height` | Image dimensions |
| `GdipGetImagePixelFormat` | — | Query pixel format |
| `GdipGetImageEncoders` / `GdipGetImageEncodersSize` | — | Find available image encoders |
| `GdipSaveImageToFile` | `cairo_surface_write_to_png` / GdkPixbuf | Save image to file |

**Graphics Context:**
| Function | Linux Equivalent | Purpose |
|----------|-----------------|---------|
| `GdipCreateFromHDC` | `cairo_create(surface)` | Create graphics from device context |
| `GdipGetImageGraphicsContext` | `cairo_create(surface)` | Create graphics from image |
| `GdipDeleteGraphics` | `cairo_destroy` | Release graphics |
| `GdipGetDC` / `GdipReleaseDC` | — | Get/release raw DC from graphics |
| `GdipSetSmoothingMode` | `cairo_set_antialias` | Anti-aliasing |
| `GdipSetInterpolationMode` | `cairo_pattern_set_filter` | Image scaling quality |
| `GdipSetCompositingQuality` | `cairo_set_operator` | Compositing quality |
| `GdipSetPixelOffsetMode` | — | Sub-pixel positioning |
| `GdipSetPageUnit` | — | Set measurement unit |
| `GdiplusStartup` / `GdiplusShutdown` | — | Library init/cleanup |

**Drawing Primitives:**
| Function | Linux Equivalent | Purpose |
|----------|-----------------|---------|
| `GdipDrawLineI` | `cairo_move_to` / `cairo_line_to` / `cairo_stroke` | Draw line |
| `GdipDrawLinesI` | `cairo_move_to` + loop | Draw polyline (pencil tool) |
| `GdipDrawRectangleI` | `cairo_rectangle` / `cairo_stroke` | Draw rectangle outline |
| `GdipDrawEllipseI` | `cairo_arc` / `cairo_stroke` | Draw ellipse (marker) |
| `GdipFillEllipse` | `cairo_arc` / `cairo_fill` | Filled ellipse |
| `GdipFillPath` | `cairo_fill` | Fill arbitrary path |
| `GdipDrawImageRectI` / `GdipDrawImageRectRectI` / `GdipDrawImageRectRect` | `cairo_set_source_surface` + `cairo_paint` | Draw image to region |

**Pen & Arrow (Arrow Annotation Tool):**
| Function | Linux Equivalent | Purpose |
|----------|-----------------|---------|
| `GdipCreatePen1` | `cairo_set_line_width` + `cairo_set_source_rgba` | Create pen |
| `GdipDeletePen` | — | Release pen |
| `GdipSetPenColor` | `cairo_set_source_rgba` | Change pen color |
| `GdipSetPenWidth` | `cairo_set_line_width` | Change pen width |
| `GdipSetPenStartCap` / `GdipSetPenEndCap` | `cairo_set_line_cap` | Line end caps |
| `GdipSetPenLineJoin` | `cairo_set_line_join` | Line join style |
| `GdipCreateAdjustableArrowCap` | Custom Cairo path | Arrow cap for arrow tool |
| `GdipSetPenCustomEndCap` | Custom drawing | Apply arrow cap |
| `GdipDeleteCustomLineCap` | — | Release cap |

**Text Rendering (Text Tool):**
| Function | Linux Equivalent | Purpose |
|----------|-----------------|---------|
| `GdipDrawString` | `pango_cairo_show_layout` | Render text |
| `GdipMeasureString` | `pango_layout_get_pixel_size` | Measure text bounds |
| `GdipCreateFont` | `pango_font_description_new` | Create font |
| `GdipCreateFontFamilyFromName` | `pango_font_description_set_family` | Set font family |
| `GdipGetGenericFontFamilySansSerif` | — | Get default sans-serif |
| `GdipDeleteFont` / `GdipDeleteFontFamily` | `pango_font_description_free` | Release font |
| `GdipCreateStringFormat` / `GdipDeleteStringFormat` | `pango_layout_set_alignment` | String formatting |
| `GdipSetStringFormatAlign` | `pango_layout_set_alignment` | Text alignment |

**Brush & Color:**
| Function | Linux Equivalent | Purpose |
|----------|-----------------|---------|
| `GdipCreateSolidFill` | `cairo_set_source_rgba` | Create solid brush |
| `GdipCloneBrush` / `GdipDeleteBrush` | — | Clone/release brush |
| `GdipSetImageAttributesColorMatrix` | Cairo operators | Color matrix transform (semi-transparency) |
| `GdipCreateImageAttributes` / `GdipDisposeImageAttributes` | — | Image attributes |

**Path Operations:**
| Function | Linux Equivalent | Purpose |
|----------|-----------------|---------|
| `GdipCreatePath` | `cairo_new_path` | Begin path |
| `GdipResetPath` | `cairo_new_path` | Clear path |
| `GdipClosePathFigure` | `cairo_close_path` | Close subpath |
| `GdipDeletePath` | — | Release path |

**Memory:**
| Function | Purpose |
|----------|---------|
| `GdipAlloc` / `GdipFree` | GDI+ memory management |

### 1.2 GDI32.dll — Screen Capture & Bitmap Manipulation

| Function | Category | Purpose |
|----------|----------|---------|
| `BitBlt` | **Screen Capture** | Primary screen capture via bit block transfer |
| `CreateCompatibleDC` | Capture | Create memory device context |
| `CreateCompatibleBitmap` | Capture | Create bitmap compatible with screen DC |
| `SelectObject` | Capture | Select bitmap into DC |
| `DeleteDC` / `DeleteObject` | Cleanup | Release resources |
| `CreateDIBSection` | Pixel Access | Create DIB for direct pixel access |
| `GetDIBits` | Pixel Access | Extract raw pixel data |
| `GetDeviceCaps` | Screen Info | Query display capabilities |
| `GetObjectW` | Bitmap Info | Get bitmap properties |
| `GetStockObject` | Drawing | Get stock GDI objects |
| `CreateFontW` | Text | Create GDI font |
| `GetTextMetricsW` | Text | Query font metrics |
| `SetBkMode` / `SetBkColor` | Text | Text background mode |
| `SetTextColor` | Text | Text color |
| `ExtTextOutW` | Text | Extended text output |
| `CreatePen` | Drawing | Create GDI pen |
| `CreateSolidBrush` | Drawing | Create solid brush |
| `CreateBitmap` | Drawing | Create bitmap |
| `Rectangle` | Drawing | Draw rectangle |
| `StretchBlt` | Printing | Stretch blit for print scaling |
| `CreateRectRgnIndirect` / `CombineRgn` / `EqualRgn` / `RectInRegion` | Regions | Monitor region operations |
| `StartDocW` / `EndDoc` / `StartPage` / `EndPage` | Printing | Print job control |

> **Key Insight**: `BitBlt` from `GetWindowDC`/`GetDC` is the primary GDI-based capture method.
> Linux equivalent: XGetImage / XShmGetImage or portal-based screenshots.

### 1.3 USER32.dll — Window Management & Input

**Screen Capture Related:**
| Function | Purpose |
|----------|---------|
| `GetWindowDC` | Get DC for entire window/screen |
| `GetDC` / `ReleaseDC` | Get/release device context |
| `GetDesktopWindow` | Get desktop window handle |
| `EnumDisplayMonitors` | Enumerate all monitors |
| `GetSystemMetrics` | Screen dimensions |
| `MonitorFromPoint` | Get monitor from screen coordinates |
| `GetMonitorInfoW` | Monitor dimensions and work area |
| `ScreenToClient` / `ClientToScreen` | Coordinate conversion |

**Selection Overlay Window:**
| Function | Purpose |
|----------|---------|
| `CreateWindowExW` | Create overlay window |
| `RegisterClassExW` | Register window class |
| `DefWindowProcW` / `CallWindowProcW` | Window procedure |
| `SetWindowPos` | Position/resize/layer overlay |
| `ShowWindow` / `UpdateWindow` | Show/refresh window |
| `MoveWindow` | Reposition window |
| `DestroyWindow` | Destroy window |
| `InvalidateRect` / `RedrawWindow` | Force repaint |
| `SetWindowLongW` / `GetWindowLongW` | Window styles |

**Input Handling:**
| Function | Purpose |
|----------|---------|
| `GetCursorPos` | Current mouse position |
| `GetAsyncKeyState` | Real-time key state |
| `SetCapture` / `ReleaseCapture` | Mouse capture for drag |
| `SetCursor` | Change cursor shape |
| `LoadCursorW` | Load cursor resource |
| `TrackMouseEvent` | Mouse enter/leave tracking |
| `CreateCaret` / `ShowCaret` | Text cursor in text tool |

**Clipboard:**
| Function | Purpose |
|----------|---------|
| `OpenClipboard` | Open clipboard |
| `EmptyClipboard` | Clear clipboard |
| `SetClipboardData` | Set clipboard image (CF_BITMAP) |
| `CloseClipboard` | Close clipboard |

**Mouse Cursor Capture:**
| Function | Purpose |
|----------|---------|
| `GetCursorInfo` | Get cursor visibility & position |
| `GetIconInfo` | Get cursor icon details |
| `DrawIcon` | Render cursor onto screenshot |

**UI Elements:**
| Function | Purpose |
|----------|---------|
| `CreatePopupMenu` / `AppendMenuW` / `TrackPopupMenu` | Context menu |
| `CreateDialogParamW` / `DialogBoxParamW` | Modal/modeless dialogs |
| `SendMessageW` / `PostMessageW` | Inter-window messaging |
| `FindWindowW` | Find existing Lightshot window |
| `BeginPaint` / `EndPaint` / `FillRect` / `DrawTextW` | Custom painting |
| `SetFocus` / `SetForegroundWindow` | Focus management |

### 1.4 COMDLG32.dll — Common Dialogs

| Function | Purpose |
|----------|---------|
| `GetSaveFileNameW` | Save As dialog |
| `GetOpenFileNameW` | Open file dialog |
| `PrintDlgExW` | Print dialog |
| `ChooseColorW` | Color picker dialog |

### 1.5 SHELL32.dll

| Function | Purpose |
|----------|---------|
| `SHGetSpecialFolderPathW` | Get special folders (Desktop, Pictures, etc.) |

### 1.6 ADVAPI32.dll — Registry

| Function | Purpose |
|----------|---------|
| `RegCreateKeyExW` | Create/open registry key |
| `RegOpenKeyExW` | Open registry key |
| `RegQueryValueExW` | Read registry value |
| `RegSetValueExW` | Write registry value |
| `RegDeleteKeyW` / `RegDeleteValueW` | Delete key/value |
| `RegEnumKeyExW` | Enumerate subkeys |
| `RegCloseKey` | Close key handle |
| `RegQueryInfoKeyW` | Query key metadata |

### 1.7 ole32.dll / OLEAUT32.dll

| Function | Purpose |
|----------|---------|
| `CreateStreamOnHGlobal` | Create IStream from memory (for GDI+ image loading) |
| `CoCreateInstance` | COM object creation |
| `CoTaskMemAlloc` / `CoTaskMemFree` / `CoTaskMemRealloc` | COM memory |
| Ordinal 277 (OLEAUT32) | Likely `SysAllocString` or similar |

### 1.8 SHLWAPI.dll — Path Utilities

| Function | Purpose |
|----------|---------|
| `PathAppendW` | Append path component |
| `PathFileExistsW` | Check file existence |
| `PathFindFileNameW` | Get filename from path |
| `PathRemoveFileSpecW` | Remove filename from path |
| `PathAddBackslashW` | Ensure trailing separator |
| `PathCombineW` | Combine path components |

---

## 2. DXGIODScreenshot.dll — DXGI Desktop Duplication

### 2.1 Direct3D 11 (d3d11.dll)

| Function | Purpose |
|----------|---------|
| `D3D11CreateDevice` | Create D3D11 device for DXGI access |

### 2.2 DXGI (dxgi.dll)

| Function | Purpose |
|----------|---------|
| `CreateDXGIFactory1` | Create DXGI factory for output enumeration |

> **Architecture Note**: After creating the factory/device, the DLL uses COM interfaces
> (IDXGIOutputDuplication) via vtable calls — not visible in import table.
> The flow is: CreateDXGIFactory1 → EnumAdapters → EnumOutputs → 
> QueryInterface(IDXGIOutput1) → DuplicateOutput → AcquireNextFrame.

### 2.3 GDI32.dll (Fallback Path)

| Function | Purpose |
|----------|---------|
| `BitBlt` | GDI capture fallback |
| `CreateCompatibleDC` / `CreateCompatibleBitmap` | Memory DC |
| `SelectObject` / `DeleteObject` / `DeleteDC` | Resource management |
| `CreateRectRgnIndirect` / `CombineRgn` / `EqualRgn` | Multi-monitor region handling |

### 2.4 USER32.dll

| Function | Purpose |
|----------|---------|
| `EnumDisplayMonitors` | Enumerate all connected monitors |
| `CopyRect` | Rectangle copy |
| `GetWindowDC` / `ReleaseDC` | Get screen DC for GDI capture |

### 2.5 Version Checking

- `VerifyVersionInfoW` / `VerSetConditionMask` — checks Windows version (DXGI Desktop Duplication requires Win8+)

---

## 3. uploader.dll — Image Upload

### 3.1 Key Imports

| DLL | Function | Purpose |
|-----|----------|---------|
| VERSION.dll | `GetFileVersionInfoW`, `VerQueryValueW`, `GetFileVersionInfoSizeW` | Get app version for user-agent |
| WININET.dll | `InternetQueryOptionW` | Query proxy settings |
| GDI32.dll | `GetDeviceCaps` | Screen DPI for thumbnail |
| SHLWAPI.dll | Path manipulation functions | File path handling |

### 3.2 Upload Protocol (from strings)

Multipart form-data POST to `https://upload.prntscr.com/upload`:
```
Content-Disposition: form-data; name="%s"; filename="%s"
Content-Type: %s
multipart/form-data; boundary=
```

Fields: `image`, `thumb`, `app_token`, `app_id`, `True` (flag)
Response: JSON with `status`, `success`, `direct_link`, `width`, `height`

---

## 4. net.dll — Network Layer

### 4.1 Dual HTTP Engine

**libcurl 7.57.0** (statically linked):
- Full curl library embedded (identified by `CLIENT libcurl 7.57.0` string)
- SSL/TLS via Windows crypto APIs (CryptAcquireContext, CryptEncrypt, CryptImportKey)
- Supports: HTTP, HTTPS, FTP, SOCKS4/5 proxies

**WinInet** (dynamically linked):
| Function | Purpose |
|----------|---------|
| `InternetOpenW` | Initialize WinInet |
| `InternetConnectW` | Connect to server |
| `HttpOpenRequestW` | Create HTTP request |
| `HttpAddRequestHeadersW` | Add custom headers |
| `HttpSendRequestW` / `HttpSendRequestExW` | Send request |
| `HttpEndRequestW` | End chunked request |
| `HttpQueryInfoW` | Query response info |
| `InternetReadFile` | Read response body |
| `InternetWriteFile` | Write request body (chunked upload) |
| `InternetOpenUrlW` | Open URL directly |
| `InternetCloseHandle` | Close handle |
| `InternetQueryOptionW` | Query options (proxy) |

### 4.2 Socket Layer (WS2_32.dll)

Full Winsock2 import by ordinal — includes socket, connect, send, recv, select, bind, listen, accept, getaddrinfo, freeaddrinfo, WSAIoctl.

### 4.3 Crypto (ADVAPI32.dll)

| Function | Purpose |
|----------|---------|
| `CryptAcquireContextA` | Get crypto provider |
| `CryptCreateHash` | Create hash object |
| `CryptHashData` | Hash data |
| `CryptGetHashParam` | Get hash result |
| `CryptDestroyHash` | Release hash |
| `CryptReleaseContext` | Release provider |
| `CryptGenRandom` | Generate random bytes (for SSL) |
| `CryptImportKey` / `CryptEncrypt` / `CryptDestroyKey` | RSA operations for SSL |

### 4.4 Certificate (CRYPT32.dll)

| Function | Purpose |
|----------|---------|
| `CertFreeCertificateContext` | Free certificate (SSL validation) |

### 4.5 LDAP (WLDAP32.dll)

Imported by ordinal — likely dependency from curl's LDAP URL support (not actively used).

---

## 5. Lightshot.exe — Main Application

### 5.1 Unique Imports (beyond what DLLs import)

| Function | Purpose |
|----------|---------|
| `RegisterHotKey` | Register global hotkey (PrtSc) |
| `UnregisterHotKey` | Unregister hotkey on exit |
| `GetMessageW` / `TranslateMessage` / `DispatchMessageW` | Message loop |
| `PostQuitMessage` | Exit message loop |
| `SetTimer` / `KillTimer` | Timer for periodic tasks |
| `GetKeyNameTextW` | Convert VK code to display name |
| `MapVirtualKeyW` | Map virtual key code |
| `EnableWindow` | Enable/disable controls |
| `DialogBoxParamW` | Options/About dialogs |
| `LoadMenuW` / `GetSubMenu` / `RemoveMenu` | Menu management |
| `LoadIconW` / `DestroyIcon` | Tray icon management |
| `RegisterWindowMessageA` / `RegisterWindowMessageW` | Custom messages |
| `GetComputerNameW` | Machine name for analytics |
| `GetVersionExW` | OS version detection |
| `GetLocalTime` | Local time for analytics |
| `CreateThread` / `TerminateThread` | Background thread management |

### 5.2 WTL/ATL Framework

Confirmed WTL (Windows Template Library) usage from RTTI:
- `CPropertyPageImpl<CFormatsPage>` — property sheet pages
- `CPropertyPageImpl<CHotkeysPage>`
- `CPropertyPageImpl<CProxyPage>`
- `CDialogImpl<CSigninProgressDialog>` — modal dialog
- `CWindowImpl<CCustomHotkeyControl, CEditT>` — custom edit control

### 5.3 DPI Awareness

- Manifest: `<dpiAware>True/PM</dpiAware>` (per-monitor DPI aware)
- `Shcore.dll` referenced (loaded dynamically for `SetProcessDpiAwareness`)

---

## 6. API Pattern Summary for Linux Reimplementation

| Lightshot Capability | Windows API | Linux Equivalent |
|---------------------|-------------|------------------|
| Screen capture (GDI) | `BitBlt` from `GetWindowDC` | XGetImage, XShmGetImage, or PipeWire portal |
| Screen capture (DXGI) | `D3D11CreateDevice` + Desktop Duplication | PipeWire, wlr-screencopy, KDE portal |
| Multi-monitor enum | `EnumDisplayMonitors` | Xinerama, XRandR, or Wayland output enumeration |
| Selection overlay | Transparent `CreateWindowExW` | Transparent GTK/Qt window or Wayland layer-shell |
| Annotation drawing | GDI+ (GdipDraw*) | Cairo / QPainter |
| Text rendering | GDI+ (GdipDrawString) | Pango+Cairo / QFont+QPainter |
| Arrow tool | `GdipCreateAdjustableArrowCap` | Custom Cairo path drawing |
| Color picker | `ChooseColorW` | GtkColorChooserDialog / QColorDialog |
| Save dialog | `GetSaveFileNameW` | GtkFileChooserDialog / QFileDialog |
| Print | `PrintDlgExW` + GDI print | GTK print / CUPS |
| Clipboard | `OpenClipboard` + `SetClipboardData` | X11 selections / Wayland data-device |
| Global hotkey | `RegisterHotKey` | XGrabKey / global shortcut portal |
| System tray | Shell_NotifyIcon (not imported directly) | GtkStatusIcon / libappindicator / SNI |
| HTTP upload | WinInet / libcurl | libcurl / libsoup |
| Registry config | `RegSetValueExW` etc. | GSettings / XDG config files |
| Image encoding | GDI+ encoders | libpng / libjpeg / GdkPixbuf |
| File paths | `SHGetSpecialFolderPathW` | XDG directories |
| DPI scaling | `Shcore.dll` / per-monitor | GDK/Qt DPI awareness |

