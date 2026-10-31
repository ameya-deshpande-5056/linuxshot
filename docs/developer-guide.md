# LinuxShot Developer Guide

## Architecture Overview

LinuxShot is built with modern C++20 and Qt 6.4+. It follows a modular design with separation between platform integration, UI presentation, command history, and capture backends.

```
src/
├── main.cpp                  # Application entry point, CLI parser, single instance
├── core/
│   └── Application           # Main coordinator orchestrating modules
├── settings/
│   └── SettingsManager       # XDG-compliant persistent configuration (~/.config/linuxshot)
├── platform/
│   ├── Platform              # Platform, session, and desktop environment detection
│   └── GlobalShortcutManager # XCB global hotkey grabber & native event filter
├── capture/
│   ├── CaptureBackend        # Abstract interface for screen capture
│   ├── CaptureManager        # Backend selector (X11 vs Wayland/Portal)
│   ├── X11CaptureBackend     # XCB-SHM zero-copy capture & multi-monitor stitcher
│   └── PortalCaptureBackend  # Freedesktop portal D-Bus capture backend
├── annotation/
│   ├── AnnotationCommand     # Command pattern base for non-destructive edits
│   ├── CommandHistory        # Undo/redo stack
│   ├── AnnotationTool        # Abstract tool state machine
│   ├── PenTool               # Freehand bezier/polyline tool
│   ├── LineTool              # Two-point line tool
│   ├── ArrowTool             # Directional arrow with vector head
│   ├── RectTool              # Rectangle border tool
│   ├── CircleTool            # Ellipse outline tool
│   ├── MarkerTool            # Semi-transparent highlighter tool
│   ├── TextTool              # Outlined text tool
│   └── BlurTool              # In-place pixel mosaic block averaging
├── export/
│   ├── ImageExporter         # Multi-format codec wrapper (PNG/JPEG/BMP)
│   └── ExportManager         # High-level save dialog & clipboard dispatch
├── upload/
│   ├── UploadProvider        # Abstract upload interface
│   ├── UploadManager         # Upload orchestration & proxy management
│   └── SelfHostedProvider    # Asynchronous multipart HTTP POST provider
└── ui/
    ├── ColorButton           # Swatch button opening color chooser
    ├── Toolbar               # Floating dark toolbar with tool and action items
    ├── AnnotationEditor      # Interactive canvas widget
    ├── SelectionOverlay      # Fullscreen dimming overlay with 8 resize handles
    ├── SettingsDialog        # Multi-tab preferences window
    └── TrayIcon              # System tray integration and context menu
```

---

## Coding Standards

1. **C++ Standard**: C++20 (`-std=c++20`).
2. **Ownership**: Explicit RAII and smart pointers (`std::unique_ptr`, `std::shared_ptr`). No raw owning pointers.
3. **Qt Guidelines**: Use type-safe pointer-to-member syntax for `QObject::connect()`.
4. **Safety**: Compile with `-Wall -Wextra -Wpedantic`. AddressSanitizer and UndefinedBehaviorSanitizer options supported via `-DENABLE_ASAN=ON` and `-DENABLE_UBSAN=ON`.
5. **No UI Blocking**: All I/O, networking, and platform communication is asynchronous.

---

## Building & Testing

### Prerequisites (Debian / Ubuntu / Mint)
```bash
sudo apt install build-essential cmake qt6-base-dev qt6-base-dev-tools libxcb1-dev libxcb-shm0-dev libxcb-keysyms1-dev
```

### Build Instructions
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### Running Automated Unit Tests
```bash
ctest --verbose
```

All 4 test suites must pass:
1. `CommandHistory`: Verifies undo, redo, stack clears, and invalidation.
2. `Settings`: Verifies configuration defaults, custom file isolation, and quality bounds.
3. `ImageExporter`: Tests PNG, JPEG, BMP encodings, invalid image handling, and naming.
4. `AnnotationTools`: Verifies drawing execution and pixel verification for all 8 tools.

---

## Packaging

### Native Debian Package (`.deb`)
```bash
mkdir -p build/deb/linuxshot_0.1.0_amd64/DEBIAN
mkdir -p build/deb/linuxshot_0.1.0_amd64/usr/bin
mkdir -p build/deb/linuxshot_0.1.0_amd64/usr/share/applications
mkdir -p build/deb/linuxshot_0.1.0_amd64/usr/share/icons/hicolor/256x256/apps
mkdir -p build/deb/linuxshot_0.1.0_amd64/usr/share/icons/hicolor/scalable/apps

cp build/linuxshot build/deb/linuxshot_0.1.0_amd64/usr/bin/
cp packaging/linuxshot.desktop build/deb/linuxshot_0.1.0_amd64/usr/share/applications/
cp resources/icons/linuxshot.png build/deb/linuxshot_0.1.0_amd64/usr/share/icons/hicolor/256x256/apps/
cp resources/icons/linuxshot.svg build/deb/linuxshot_0.1.0_amd64/usr/share/icons/hicolor/scalable/apps/

dpkg-deb --build --root-owner-group build/deb/linuxshot_0.1.0_amd64 packaging/deb/linuxshot_0.1.0_amd64.deb
```

### AppImage Package
Uses standard Type-2 AppImage container with squashfs:
```bash
mksquashfs build/AppDir /tmp/linuxshot.squashfs -root-owned -noappend -comp gzip
cat /tmp/appimage_runtime /tmp/linuxshot.squashfs > packaging/appimage/LinuxShot-0.1.0-x86_64.AppImage
chmod +x packaging/appimage/LinuxShot-0.1.0-x86_64.AppImage
```

