# Changelog

All notable changes to LinuxShot will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.1] - 2026-10-31

### Added
- Interactive inline text annotation box (`FloatingTextEditor`) supporting on-canvas typing, live preview, and dynamic multi-line height expansion.
- Draggable text box positioning: drag by the dashed cyan border or top header (`Qt::SizeAllCursor`) anywhere across the screenshot canvas before committing.
- Multi-line typography rendering with automatic line spacing and high-contrast outline in `TextCommand::paint`.
- Fail-safe text commitment: active inline text automatically commits upon clicking outside the box, pressing `Ctrl+Enter`, switching tools, or executing save, copy, or upload actions.
- Dedicated dark-themed `AboutDialog` displaying a 112×112 application icon, version, Git commit hash, ISO-8601 UTC build timestamp, Qt runtime version, and clickable GitHub project URL.
- Comprehensive unit test `testMultiLineTextTool` in `tests/test_annotation_tools.cpp`.

### Changed
- Replaced modal `QInputDialog` text entry with seamless non-blocking inline text editing directly on the annotation canvas.
- Replaced standard `QMessageBox::about` with custom-styled `AboutDialog`.

## [0.1.0] - 2026-10-31

### Added
- Clean-room C++20 and Qt 6 implementation of LightShot.
- X11 zero-copy XCB-SHM capture engine and D-Bus XDG Desktop Portal fallback.
- Interactive multi-monitor selection overlay with 8 resize handles and dimension badge.
- Vector annotation engine: Pen, Line, Arrow, Rectangle, Circle, Marker (highlighter), Text, and Blur / Pixelate.
- Full Undo/Redo command history stack (`CommandHistory`).
- Image exporters for PNG, JPEG, and BMP formats.
- Pluggable asynchronous upload engine with self-hosted server support.
- System tray integration with quick capture actions and preferences.
- XDG-compliant persistent configuration (`~/.config/linuxshot/linuxshot.conf`).
- Complete automated test suite covering command history, settings, codecs, tools, and platform detection.
- Debian (`.deb`), AppImage, and Flatpak packaging targets.
- Automated system management scripts: `scripts/build.sh`, `scripts/install.sh`, and `scripts/uninstall.sh`, along with CMake `uninstall` target.

### Changed
- Real-time toolbar tracking during selection resize and move operations.
- Non-destructive crop updating preserving user annotations across selection resize events.
- Reorganized codebase into dedicated `src/platform/` and `src/settings/` directories.
- Switched UI toolbar icons to custom SVGs in `resources/icons/linuxshot-ui-svgs` compiled into Qt's resource system (`resources.qrc`), with dynamic dark-mode tinting.
- Fixed blur/pixelate toolbar icon dark-mode tinting to match other vector tool icons.
- Removed legacy extracted `resources/icons/lightshot` directory.
- Fixed "Save As" modal dialog accessibility lockup by ensuring the fullscreen overlay unmaps and dismisses before spawning file chooser.

