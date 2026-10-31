# AGENTS.md — Autonomous Agent & Developer Guide for LinuxShot

Welcome to **LinuxShot**. This document provides an exhaustive, authoritative guide for autonomous AI agents, automated development pipelines, and human developers contributing to or maintaining this codebase.

---

## 1. Project Mission & Core Principles

**LinuxShot** is a clean-room, native Linux screenshot and image-annotation application. It delivers the fast, intuitive region-capture and on-screen editing experience popularized by Lightshot, built specifically for the modern Linux desktop ecosystem.

### Core Architectural Principles
1. **Native Performance**: Built with modern C++20 and Qt 6. No Electron, web views, or heavy runtime bloat.
2. **Dual-Stack Capture Engine**:
   - Zero-copy **XCB-SHM** (Shared Memory) screen capture for instant frame acquisition on X11.
   - Transparent D-Bus **XDG Desktop Portal** (`org.freedesktop.portal.Screenshot`) fallback for Wayland compositors.
3. **Interactive On-Screen Vector Annotations**: Full vector command history (`CommandHistory`) with unlimited Undo/Redo across 8 distinct annotation tools.
4. **Self-Contained UI**: Zero dependencies on desktop environment icon themes or system fonts. All UI assets are clean SVGs embedded in the Qt binary via `.qrc`.
5. **Clean-Room Boundaries**: Strictly independent code and licensed assets. No proprietary Windows binaries or copyrighted resources are ever incorporated.

---

## 2. Directory Structure & Component Map

```
linuxshot/
├── .github/
│   └── workflows/
│       ├── ci.yml               # Continuous Integration (build, test, verify packages)
│       └── release.yml          # GitHub Release automation with changelog extraction
├── CMakeLists.txt               # Main CMake build definition & test configuration
├── AGENTS.md                    # This developer & agent guide
├── ARCHITECTURE.md              # Detailed software architectural blueprint
├── CHANGELOG.md                 # Keep a Changelog compliant release history (MANDATORY)
├── PROJECT_STATUS.md            # Live implementation tracking and phase status
├── packaging/
│   ├── build-deb.sh             # Reproducible Debian (.deb) builder
│   ├── build-appimage.sh        # Reproducible Type-2 AppImage builder
│   ├── linuxshot.desktop        # FreeDesktop application entry
│   ├── deb/                     # Output directory for Debian packages
│   ├── appimage/                # Output directory for AppImage binaries
│   └── flatpak/                 # Flatpak build manifests
├── resources/
│   ├── resources.qrc            # Qt Resource System definition
│   └── icons/
│       ├── linuxshot.svg        # Scalable application icon
│       ├── linuxshot.png        # 256x256 application icon
│       └── linuxshot-ui-svgs/   # UI action and tool vector SVGs
├── src/
│   ├── main.cpp                 # Entry point, CLI args, single-instance lock
│   ├── core/
│   │   ├── Application.h/cpp    # Main controller, coordinator, hotkey/tray wiring
│   ├── capture/
│   │   ├── CaptureBackend.h     # Abstract capture engine interface
│   │   ├── CaptureManager.h/cpp # Backend selector & desktop screen grabber
│   │   ├── X11CaptureBackend.h/cpp  # High-speed XCB-SHM capture implementation
│   │   └── PortalCaptureBackend.h/cpp # D-Bus Desktop Portal implementation
│   ├── annotation/
│   │   ├── AnnotationTool.h     # Base class for vector annotation tools
│   │   ├── CommandHistory.h/cpp # Undo/Redo command stack & undo limit
│   │   ├── PenTool.h/cpp        # Freehand pencil
│   │   ├── LineTool.h/cpp       # Straight line
│   │   ├── ArrowTool.h/cpp      # Directional arrow with arrowhead
│   │   ├── RectTool.h/cpp       # Hollow rectangle outline
│   │   ├── CircleTool.h/cpp     # Ellipse / circle outline
│   │   ├── MarkerTool.h/cpp     # Semi-transparent highlighter
│   │   ├── TextTool.h/cpp       # Text annotation with dialog input
│   │   └── BlurTool.h/cpp       # Pixelate / blur rectangle
│   ├── ui/
│   │   ├── SelectionOverlay.h/cpp   # Fullscreen region selector with 8 resize handles
│   │   ├── AnnotationEditor.h/cpp   # Drawing surface & render pipeline
│   │   ├── Toolbar.h/cpp            # Floating tool and action button palettes
│   │   ├── IconHelper.h/cpp         # Vector/SVG icon provider with dark-mode tinting
│   │   ├── ColorButton.h/cpp        # Color palette dropdown widget
│   │   ├── SettingsDialog.h/cpp     # Preferences dialog
│   │   └── TrayIcon.h/cpp           # System tray integration
│   ├── platform/
│   │   ├── Platform.h/cpp           # X11/Wayland detection, DE detection, Portal check
│   │   └── GlobalShortcutManager.h/cpp # XCB global hotkey grabber
│   ├── settings/
│   │   └── SettingsManager.h/cpp    # Persistent INI config (~/.config/linuxshot/linuxshot.conf)
│   ├── export/
│   │   ├── ImageExporter.h/cpp      # PNG, JPEG, BMP encoders and default naming
│   │   └── ExportManager.h/cpp      # Save dialog & clipboard management
│   └── upload/
│       ├── UploadProvider.h/cpp     # Abstract upload provider
│       ├── UploadManager.h/cpp      # Asynchronous network multipart uploader
│       └── SelfHostedProvider.h/cpp # Custom HTTP endpoint provider
└── tests/
    ├── test_command_history.cpp # Command stack & undo/redo unit tests
    ├── test_settings.cpp        # Persistent configuration unit tests
    ├── test_image_exporter.cpp  # Image encoding unit tests
    ├── test_annotation_tools.cpp# Vector annotation tools geometry tests
    ├── test_platform.cpp        # Session and environment detection tests
    └── test_icons.cpp           # SVG and vector icon rendering tests
```

---

## 3. Critical Invariants Every Agent Must Obey

When modifying this repository, **you must never violate these core invariants**:

### 1. Window Management & Modality Invariant
- **Rule**: `SelectionOverlay` is fullscreen and runs with `Qt::WindowStaysOnTopHint`. Whenever an action triggers a modal dialog (such as `QFileDialog` for "Save As" or `SettingsDialog`), the overlay **MUST** call:
  ```cpp
  hide();
  close();
  QGuiApplication::processEvents();
  ```
  **BEFORE** emitting the signal or spawning the dialog.
- **Why**: If `SelectionOverlay` is not unmapped first, the window manager enforces `WindowStaysOnTopHint`, trapping the modal dialog underneath the fullscreen overlay. The application will appear frozen and inaccessible.

### 2. Real-Time Toolbar Tracking Invariant
- **Rule**: During selection resizing or moving, floating toolbars (`m_toolBar` and `m_actionBar`) must call `updateToolbarsPosition()` in `mouseMoveEvent` and remain raised (`raise()`) to follow the selection boundaries continuously in real time.
- **Why**: Resizing must be fluid and responsive; toolbars must never wait until mouse release to jump to the new coordinates.

### 3. Non-Destructive Selection Crop Updating
- **Rule**: When resizing an existing capture region, call `AnnotationEditor::updateCrop(rect, image)` rather than `setBaseImage()`.
- **Why**: `setBaseImage()` clears the `CommandHistory` undo stack. `updateCrop()` updates the base geometry while preserving active user annotations.

### 4. Self-Contained SVG Icon Pipeline
- **Rule**: All UI icons reside in `resources/icons/linuxshot-ui-svgs/` and are compiled into the binary via `resources/resources.qrc`.
- **Rule**: `IconHelper::loadSvgIcon` dynamically tints monochrome `#000000` SVGs to `#E2E8F0` for dark-mode toolbar contrast, while preserving custom color gradients (such as `cancel.svg`). Never hardcode paths to external files on the host filesystem.

---

## 4. Building, Testing, and Packaging

### Local Compilation
```bash
# Configure Release build with tests enabled
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON

# Compile using all available CPU cores
cmake --build build -j$(nproc)
```

### Running the Test Suite
All tests must pass with 100% success before committing:
```bash
ctest --test-dir build --output-on-failure
```

### Building Distribution Packages
LinuxShot provides standalone packaging scripts that run both locally and in CI:
```bash
# Build Debian package (.deb) -> packaging/deb/linuxshot_<version>_amd64.deb
./packaging/build-deb.sh 0.1.0

# Build AppImage (.AppImage) -> packaging/appimage/LinuxShot-<version>-x86_64.AppImage
./packaging/build-appimage.sh 0.1.0
```

---

## 5. The Release Process & The Critical Importance of CHANGELOG.md

### Why CHANGELOG.md is Mandatory
The repository's GitHub Actions release workflow (`.github/workflows/release.yml`) is **strictly coupled** to `CHANGELOG.md`:

1. **Automated Release Notes**: When a release tag (e.g. `v0.2.0`) is pushed, the workflow parses `CHANGELOG.md` to extract the exact markdown section matching `## [0.2.0] - YYYY-MM-DD`. This extracted text becomes the official GitHub Release body.
2. **Release Blocker on Missing Changelog**: If `CHANGELOG.md` does **not** contain an entry for the tagged version, the workflow **aborts immediately with an error**. No release is published.
3. **Auditing & Traceability**: Clean-room development requires continuous, traceable documentation. A missing changelog entry compromises release integrity.

### Release Protocol for Future Agents
When tasked with releasing a new version or preparing an update, follow this strict 6-step checklist:

1. **Step 1: Bump Version in CMakeLists.txt**
   Update the project version at line 2 of `CMakeLists.txt`:
   ```cmake
   project(linuxshot VERSION 0.2.0 LANGUAGES CXX)
   ```

2. **Step 2: Update CHANGELOG.md (MANDATORY)**
   Add a new version section at the top of `CHANGELOG.md` under `# Changelog`, using standard [Keep a Changelog](https://keepachangelog.com/) format:
   ```markdown
   ## [0.2.0] - 2026-11-15

   ### Added
   - List of newly implemented features...

   ### Changed
   - List of modified behavior or enhancements...

   ### Fixed
   - List of bug fixes...

   ### Removed
   - List of deprecated or removed capabilities...
   ```

3. **Step 3: Update PROJECT_STATUS.md**
   Document the current milestone and package build artifacts in `PROJECT_STATUS.md`.

4. **Step 4: Run Tests & Verify Packages Locally**
   ```bash
   cmake --build build -j$(nproc)
   ctest --test-dir build --output-on-failure
   ./packaging/build-deb.sh 0.2.0
   ./packaging/build-appimage.sh 0.2.0
   ```

5. **Step 5: Commit and Tag**
   ```bash
   git add CMakeLists.txt CHANGELOG.md PROJECT_STATUS.md
   git commit -m "chore(release): prepare v0.2.0"
   git tag -a v0.2.0 -m "Release v0.2.0"
   ```

6. **Step 6: Push to GitHub**
   ```bash
   git push origin main --tags
   ```

Once pushed, `.github/workflows/release.yml` will automatically:
- Verify that `CHANGELOG.md` has an entry for `[0.2.0]`.
- Build the binary on Ubuntu 24.04 runner.
- Run all 6 test suites.
- Build `packaging/deb/linuxshot_0.2.0_amd64.deb`.
- Build `packaging/appimage/LinuxShot-0.2.0-x86_64.AppImage`.
- Generate `packaging/SHA256SUMS.txt`.
- Publish the GitHub Release with the extracted changelog notes and all 3 release assets attached.
