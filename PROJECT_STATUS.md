# LinuxShot Project Status

**Current Status**: Complete & Verified (v0.1.4)
**Date**: 2026-11-06

---

## 1. Environment & Host Information

| Component | Detected Specification |
|---|---|
| OS | Linux Mint 22.3 (Zena) / Ubuntu 24.04 (Noble) |
| Kernel | Linux 7.0.0-38-generic x86_64 |
| Desktop Environment | Cinnamon (X-Cinnamon) |
| Windowing System | X11 (:0) with Wayland Portal support |
| Compiler | GCC 13.3.0 (C++20 enabled) |
| Build Tool | CMake 3.28.3 |
| Framework | Qt 6.4.2 (Base, GUI, Widgets, Network, DBus, Test) |
| Architecture | x86_64 |

---

## 2. Reverse-Engineering Phase Summary

- **Source File**: `setup-lightshot.exe` (2,786,328 bytes, Inno Setup 5.6.0).
- **Extraction**: Clean extraction via `innoextract 1.9` into `research/extracted/`.
- **Binaries Analyzed**:
  - `app/5.5.0.7/Lightshot.exe` (WTL/ATL host process)
  - `app/5.5.0.7/Lightshot.dll` (GDI/GDI+ screenshot and annotation core)
  - `app/5.5.0.7/DXGIODScreenshot.dll` (DirectX 11 desktop duplication)
  - `app/5.5.0.7/uploader.dll` (Image upload and social sharing)
  - `app/5.5.0.7/net.dll` (libcurl 7.57.0 networking)
- **Artifacts Produced**:
  - `research/installer-analysis.md`
  - `research/file-inventory.md`
  - `research/static-analysis.md`
  - `research/api-analysis.md`
  - `research/binary-architecture.md`
  - `research/open-questions.md`
  - `research/behavioural-analysis.md`
  - `research/ui-inventory.md`
  - `research/keyboard-shortcuts.md`
  - `research/settings-inventory.md`
  - `research/feature-matrix.md`

---

## 3. Implementation Verification & Features

All features implemented clean-room in modern C++20 with Qt 6:

- [x] **Desktop Capture**:
  - X11 zero-copy capture via `xcb_shm_get_image` with composite fallback.
  - Wayland portal capture via D-Bus `org.freedesktop.portal.Screenshot`.
  - Multi-monitor virtual desktop geometry stitching.
- [x] **Interactive Selection Interface**:
  - Fullscreen transparent window with dark scrim.
  - Interactive drag-to-select rectangle with high-contrast cyan dashed border.
  - 8 resize handles with corresponding cursor changes (`SizeBDiag`, `SizeFDiag`, `SizeVer`, `SizeHor`).
  - Move selection by dragging inside with `SizeAll` cursor.
  - Arrow key pixel nudging (1px or 10px with Shift).
  - Floating badge showing selection dimensions (`W x H px`).
  - Minimum selection size enforcement (10x10 px).
  - Keyboard shortcuts (`Ctrl+A`, `Ctrl+C`, `Ctrl+S`, `Ctrl+Z`, `Esc`).
- [x] **Vector Annotation Engine**:
  - Command pattern undo/redo stack (`CommandHistory`).
  - Pen tool (smooth freehand drawing).
  - Line tool.
  - Arrow tool (calculated vector triangle head).
  - Rectangle tool.
  - Circle/Ellipse tool.
  - Highlighter marker tool (semi-transparent 90-alpha multiply blend).
  - Text tool (interactive inline text box with drag-to-move, multi-line typography, and click-outside commitment).
  - Blur / Pixelate tool (direct pixel block mosaic averaging).
  - Color picker swatch.
- [x] **Export & Output**:
  - Multi-format exporter (`PNG`, `JPEG`, `BMP`).
  - Clipboard copy integration (`QClipboard`).
  - Save file dialog with pre-filled timestamped filenames.
  - Desktop notifications for save and copy events.
- [x] **Upload Subsystem**:
  - Asynchronous HTTP multipart uploader (`SelfHostedProvider`).
  - Optional togglable Lightshot / Prntscr upload provider (`PrntscrProvider`) with reverse-engineered token and hash calculation.
  - Public upload warning disclaimer in UI.
  - Configurable server endpoint URL for custom endpoints.
  - Automatic link clipboard copy upon successful response.
  - Proxy configuration (None, System, Manual).
- [x] **Preferences & Persistence**:
  - Tabbed settings dialog (`General`, `Hotkeys`, `Formats`, `Upload`).
  - XDG-compliant storage in `~/.config/linuxshot/linuxshot.conf`.
- [x] **System Tray Integration**:
  - Single-instance enforcement via `QLockFile`.
  - System tray icon with context menu.
  - Native styled "About LinuxShot" dialog with runtime and build metadata.
  - Global shortcut listener via `xcb_grab_key`.

---

## 4. Test Suite Results

Automated tests verified via `ctest`:

```
1/8 Test #1: CommandHistory ...................   Passed
2/8 Test #2: Settings .........................   Passed
3/8 Test #3: ImageExporter ....................   Passed
4/8 Test #4: AnnotationTools ..................   Passed
5/8 Test #5: Platform .........................   Passed
6/8 Test #6: Icons ............................   Passed
7/8 Test #7: UploadProvider ...................   Passed
8/8 Test #8: SelectionOverlay .................   Passed

100% tests passed, 0 tests failed out of 8 test suites.
```

Runtime integration verification:
- Fullscreen capture verified on live X11 display (`DISPLAY=:0`).
- Generated 1920x1080 8-bit RGB PNG screenshot successfully in `~/Pictures/LinuxShot_2026-10-31_10-13-20.png`.

---

## 5. Distribution Packages

1. **Debian / Ubuntu Package**:
   - Location: `packaging/deb/linuxshot_0.1.3_amd64.deb`
   - Size: ~140 KiB
   - Architecture: `amd64`
   - Fully packaged with desktop entry, scalable SVG icon, and dependencies.
2. **AppImage Package**:
   - Location: `packaging/appimage/LinuxShot-0.1.3-x86_64.AppImage`
   - Verified executable with `--appimage-extract-and-run --version` and `--help`.
3. **Flatpak Package & Pipeline**:
   - Manifest: `packaging/flatpak/org.linuxshot.LinuxShot.yaml`
   - Metainfo: `packaging/flatpak/org.linuxshot.LinuxShot.metainfo.xml`
   - Desktop entry: `packaging/flatpak/org.linuxshot.LinuxShot.desktop`
   - Standalone builder: `packaging/build-flatpak.sh`
   - Target bundle: `packaging/flatpak/LinuxShot-${VERSION}-x86_64.flatpak`
   - Automated CI build and release attachment in `.github/workflows/ci.yml` and `.github/workflows/release.yml`.

---

## 6. Recent Enhancements & Fixes

- **HiDPI & Screen Zoom Screen Capture**: Fixed truncated desktop captures on displays with screen zoom (> 100%) or fractional scaling. `X11CaptureBackend` now captures the full physical root window dimensions via XCB-SHM and composite fallback instead of being truncated to Qt logical bounds. `SelectionOverlay` now maps logical widget coordinates to physical screenshot dimensions when cropping and rendering, and `AnnotationEditor` properly scales vector annotations and pixelation blocks at native resolution.
- **Real-Time Toolbar Tracking**: Updated `SelectionOverlay` mouse handling so both vertical and horizontal toolbars dynamically track selection boundaries in real time as any of the 8 resize handles are dragged, rather than waiting until mouse release.
- **Child Event Filter**: Added an event filter on `AnnotationEditor` to intercept hover and press events over edge handles (expanded 14px hit-box) so resize gestures can be initiated smoothly even after annotations have started.
- **Non-Destructive Resizing**: Implemented `AnnotationEditor::updateCrop` to ensure active annotations are preserved when resizing selections.
