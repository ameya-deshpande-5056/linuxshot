# Changelog

All notable changes to LinuxShot will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.4] - 2026-11-06

### Added
- Complete Flatpak packaging pipeline and standalone bundle builder (`packaging/build-flatpak.sh`).
- AppStream metadata specification (`packaging/flatpak/org.linuxshot.LinuxShot.metainfo.xml`) for Flatpak app stores and desktop integration.
- Dedicated Flatpak desktop launcher entry (`packaging/flatpak/org.linuxshot.LinuxShot.desktop`).
- Continuous Integration Flatpak build verification job in `.github/workflows/ci.yml`.
- Automated Flatpak release packaging job in `.github/workflows/release.yml` publishing `.flatpak` bundles and checksums alongside `.deb` and `.AppImage`.
- Dedicated unit test suite `test_selection_overlay.cpp` verifying high-DPI scaling, full-desktop cropping, and sub-region coordinate mapping.
- Scaled blur unit test `testBlurToolScaled` in `tests/test_annotation_tools.cpp`.

### Fixed
- Fixed truncated screen capture on displays with display zoom (> 100%) or fractional scaling:
  - `X11CaptureBackend` now captures the root window using physical pixel dimensions rather than Qt logical screen coordinates via XCB-SHM and composite fallback.
  - `SelectionOverlay` now maps logical widget coordinates to physical screenshot dimensions using aspect scaling factors in `croppedSelectionImage()`, eliminating missing bottom and right screen areas upon selection and export.
  - `SelectionOverlay::paintEvent` now scales the desktop screenshot to the widget boundary rect to render 1:1 onto high-DPI backing stores without clipping.
  - `AnnotationEditor` now scales vector annotations and blur pixelation mosaic blocks to match native display resolution when rendering final images.

## [0.1.3] - 2026-10-31

### Added
- Optional Lightshot (`prnt.sc`) screenshot upload provider (`PrntscrProvider`) in `UploadManager`.
- Security warning disclaimer in Preferences ("Upload & Network" tab) alerting users that screenshots uploaded to `prnt.sc` are publicly viewable on the internet without authentication.
- Dynamic mutually-exclusive settings toggle between custom self-hosted server endpoints and the public `prnt.sc` provider.
- Complete automated unit test suite `test_upload_provider.cpp` verifying request signature hashing, salt determinism, and provider toggling.
- Unit tests `testTextCommandWordWrap` and `testFloatingTextEditorWordWrap` in `tests/test_annotation_tools.cpp` verifying fixed-width word-wrapping and commitment fidelity.

### Changed
- Decrypted runtime AES-128-CBC salt from Lightshot client binaries to obtain the authentic signature salt (`5CE3DF4D45AC`) and MD5 request parameter hashing for `upload.prntscr.com`.
- Refined multipart payload schema to omit `app_token` on anonymous uploads, matching server protocol requirements and preventing `Wrong params` rejection.

### Fixed
- Fixed text box word wrap vanishing upon commitment: `FloatingTextEditor` now commits text wrapped at the exact bounding box width, and `TextCommand` dynamically breaks words across lines matching on-screen editor layout via `QTextLayout`.

## [0.1.2] - 2026-10-31

### Changed
- Removed intrusive top drag header bar from `FloatingTextEditor`, eliminating vertical obstruction and allowing text boxes to be placed directly against the top margin (`y = 0`) of screenshots.
- Enhanced text box repositioning: drag by any of the 4 dashed borders (`Qt::SizeAllCursor`) or `Alt + Drag` from anywhere inside the text box.
- Updated baseline calculation and clamping bounds for pixel-accurate alignment without header offset.
- Reduced default single-click text box height from 56px to 38px.

### Added
- Unit test `testFloatingTextEditorTopMargin` in `tests/test_annotation_tools.cpp` verifying zero-margin positioning and baseline accuracy.

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

