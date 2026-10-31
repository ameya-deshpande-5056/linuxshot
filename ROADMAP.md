# LinuxShot Roadmap

## Phase 0: Environment & Project Setup ✅
- [x] Environment inspection (Linux Mint 22.3, Cinnamon, X11, GCC 13.3, Qt 6.4.2)
- [x] Project structure initialization (`src/`, `docs/`, `research/`, `tests/`, `packaging/`)
- [x] Git repository setup
- [x] Documentation scaffolding

## Phase 1: Installer Analysis & Extraction ✅
- [x] Inno Setup 5.6.0 analysis and extraction via `innoextract`
- [x] Complete file inventory and SHA-256 hash ledger (`research/file-inventory.md`)
- [x] File classification and architecture verification

## Phase 2: Static Reverse Engineering ✅
- [x] PE32 binary analysis with Cutter, rizin, and objdump
- [x] Import table investigation (GDI, DXGI, GDI+, USER32, libcurl, WinInet)
- [x] String extraction and UI feature mapping
- [x] Resource inventory & module decomposition
- [x] Research documentation (`research/static-analysis.md`, `research/api-analysis.md`, `research/binary-architecture.md`)

## Phase 3: Dynamic & Behavioural Analysis ✅
- [x] UI inventory from strings and locales (`research/ui-inventory.md`)
- [x] Feature matrix construction (`research/feature-matrix.md`)
- [x] Keyboard shortcut documentation (`research/keyboard-shortcuts.md`)
- [x] Settings inventory (`research/settings-inventory.md`)

## Phase 4: Architecture Design ✅
- [x] Component architecture specification (`ARCHITECTURE.md`)
- [x] Platform abstraction design (X11 XCB vs Wayland Portal)
- [x] Capture pipeline design
- [x] Command-based undo/redo annotation engine design
- [x] CMake 3.22+ build system setup

## Phase 5: Core Implementation ✅
- [x] Application lifecycle and single-instance enforcement (`src/core/Application.cpp`)
- [x] System tray integration (`src/ui/TrayIcon.cpp`)
- [x] X11 zero-copy screen capture (`src/capture/X11CaptureBackend.cpp`)
- [x] Wayland/Portal screen capture (`src/capture/PortalCaptureBackend.cpp`)
- [x] Fullscreen region selection overlay (`src/ui/SelectionOverlay.cpp`)
- [x] 8-handle selection resizing, moving, and pixel-nudging

## Phase 6: Annotation Engine ✅
- [x] Command pattern undo/redo stack (`src/annotation/CommandHistory.cpp`)
- [x] Pen tool (`src/annotation/PenTool.cpp`)
- [x] Line tool (`src/annotation/LineTool.cpp`)
- [x] Arrow tool (`src/annotation/ArrowTool.cpp`)
- [x] Rectangle tool (`src/annotation/RectTool.cpp`)
- [x] Circle tool (`src/annotation/CircleTool.cpp`)
- [x] Marker / Highlighter tool (`src/annotation/MarkerTool.cpp`)
- [x] Text tool (`src/annotation/TextTool.cpp`)
- [x] Blur / Pixelate tool (`src/annotation/BlurTool.cpp`)
- [x] Color picker swatch (`src/ui/ColorButton.cpp`)
- [x] Floating toolbars (`src/ui/Toolbar.cpp`)

## Phase 7: File Operations & Clipboard ✅
- [x] Save and Save As dialogs (`src/export/ExportManager.cpp`)
- [x] PNG, JPEG, and BMP image export (`src/export/ImageExporter.cpp`)
- [x] Clipboard copy integration (`QClipboard`)
- [x] Desktop notifications on save/copy

## Phase 8: Upload System ✅
- [x] Open pluggable upload provider interface (`src/upload/UploadProvider.h`)
- [x] Asynchronous multipart HTTP POST provider (`src/upload/SelfHostedProvider.cpp`)
- [x] Upload progress and automatic link clipboard copying

## Phase 9: Settings & Configuration ✅
- [x] XDG-compliant INI persistence in `~/.config/linuxshot/` (`src/core/SettingsManager.cpp`)
- [x] Tabbed settings dialog (`src/ui/SettingsDialog.cpp`)
- [x] Global shortcut configuration and XCB key grabber (`src/core/GlobalShortcutManager.cpp`)

## Phase 10: Testing ✅
- [x] Unit test: Command history undo/redo (`tests/test_command_history.cpp`)
- [x] Unit test: Settings defaults and persistence (`tests/test_settings.cpp`)
- [x] Unit test: Image exporter codec validation (`tests/test_image_exporter.cpp`)
- [x] Unit test: All 8 annotation tools drawing & pixel verification (`tests/test_annotation_tools.cpp`)
- [x] 100% CTest pass rate (4 of 4 test suites, 29 subtests passed)
- [x] Live desktop capture validation on X11 display `:0`

## Phase 11: Packaging & Distribution ✅
- [x] Native Debian/Ubuntu package (`packaging/deb/linuxshot_0.1.0_amd64.deb`)
- [x] Standalone Type-2 AppImage (`packaging/appimage/LinuxShot-0.1.0-x86_64.AppImage`)
- [x] Flatpak manifest (`packaging/flatpak/org.linuxshot.LinuxShot.yaml`)
- [x] Desktop integration files and hicolor SVG/PNG icons

## Phase 12: Documentation & Release ✅
- [x] Comprehensive User Guide (`docs/user-guide.md`)
- [x] Comprehensive Developer Guide (`docs/developer-guide.md`)
- [x] Clean-room research dossiers in `research/`
- [x] Release report
