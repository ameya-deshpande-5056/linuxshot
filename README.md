# LinuxShot

A complete, production-quality Linux-native screenshot application with region capture, annotation tools, and system integration.

LinuxShot is an independently developed screenshot tool inspired by the workflow and interaction patterns of popular screenshot applications. It provides a compact, task-focused interface for capturing, annotating, and sharing screenshots on Linux.

## Features

- **Full-screen and region capture** with intuitive selection interface
- **Annotation tools**: pen, line, arrow, rectangle, marker, text, blur/pixelation
- **Undo/redo** with command-based editing model
- **Save/export** in PNG, JPEG, and BMP formats
- **Clipboard integration** for quick sharing
- **Pluggable upload system** with self-hosted backend support
- **System tray integration** with configurable global shortcuts
- **X11 and Wayland support** via platform abstraction
- **Multi-monitor support**
- **High-DPI display scaling**
- **Configurable settings** with XDG-compliant storage

## Requirements

- Linux (X11 or Wayland)
- Qt 6.4+
- CMake 3.22+
- GCC 13+ or Clang 16+ (C++20 support)

### Runtime Dependencies

- `xdg-desktop-portal` (Wayland screenshot support)
- `pipewire` (Wayland screen capture)

## Building

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

## Installation

```bash
sudo cmake --install .
```

## Usage

Launch from the application menu or terminal:

```bash
linuxshot
```

### Keyboard Shortcuts

| Action | Default Shortcut |
|--------|-----------------|
| Capture region | Print Screen |
| Full screen capture | Shift+Print Screen |
| Cancel capture | Escape |
| Save | Ctrl+S |
| Copy to clipboard | Ctrl+C |
| Undo | Ctrl+Z |
| Redo | Ctrl+Y |

## License

GPL-3.0-or-later — see [LICENSE](LICENSE) for details.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines.

