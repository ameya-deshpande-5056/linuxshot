# LinuxShot User Guide

## Introduction

LinuxShot is a high-performance, Linux-native screenshot and annotation tool inspired by the workflow of popular screenshot utilities such as Lightshot. It enables fast region capture, full-screen capture, in-place vector annotations, one-click clipboard copying, file saving, and customizable image uploads.

---

## Installation

### Method 1: Debian / Ubuntu Package (`.deb`)
Recommended for Linux Mint, Ubuntu, Debian, Pop!_OS:

```bash
sudo dpkg -i packaging/deb/linuxshot_0.1.0_amd64.deb
# If there are any missing system dependencies:
sudo apt-get install -f
```

### Method 2: AppImage
Universal binary that runs on any modern Linux distribution without installation:

```bash
chmod +x packaging/appimage/LinuxShot-0.1.0-x86_64.AppImage

# Run directly:
./packaging/appimage/LinuxShot-0.1.0-x86_64.AppImage

# On systems without FUSE installed (e.g. Ubuntu 24.04+ default):
./packaging/appimage/LinuxShot-0.1.0-x86_64.AppImage --appimage-extract-and-run
```

### Method 3: Building from Source
Requires Qt 6.4+, CMake 3.22+, and a C++20 compiler:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
sudo cmake --install .
```

---

## First Launch

Launch LinuxShot from your desktop application launcher or run:

```bash
linuxshot
```

LinuxShot starts quietly in your system tray with a feather icon. It runs continuously in the background listening for capture shortcuts.

---

## Screenshot Capture

### 1. Interactive Region Capture
- **Shortcut**: Press `Print Screen` (or left-click the system tray icon).
- The screen dims with a translucent veil.
- **Select an Area**: Click and drag your mouse to create a rectangular selection.
- As soon as the selection is released, floating toolbars appear:
  - **Vertical Toolbar**: Annotation tools.
  - **Horizontal Toolbar**: Save, Copy, Upload, and Cancel actions.

### 2. Adjusting the Selection
- **Resize**: Drag any of the 8 square handles on corners and edges.
- **Move**: Click and drag inside the selection box to reposition it.
- **Fine-Tune**: Use the **Arrow keys** to move the selection by 1 pixel, or **Shift + Arrow keys** for 10 pixels.
- **Select Fullscreen**: Press `Ctrl + A` while capture is active.

### 3. Instant Fullscreen Capture
- Press `Shift + Print Screen` to immediately capture the entire virtual desktop across all connected monitors and save it directly to your `~/Pictures` folder.

---

## Annotation Tools

The vertical toolbar attached to your selection provides 8 annotation tools:

| Icon | Tool | Description | Shortcut |
|---|---|---|---|
| `✎` | **Pen** | Smooth freehand drawing | `P` |
| `╱` | **Line** | Straight line from start to end | `L` |
| `➔` | **Arrow** | Directional arrow with a clean arrowhead | `A` |
| `▭` | **Rectangle** | Hollow rectangular border | `R` |
| `◯` | **Circle** | Hollow elliptical border | `C` |
| `▮` | **Marker** | Semi-transparent highlighter (preserves text legibility) | `M` |
| `T` | **Text** | Place typed text annotations | `T` |
| `▓` | **Blur** | Pixelates/mosaics sensitive information (passwords, faces) | `B` |
| 🎨 | **Color** | Opens the color palette | — |
| `↶` | **Undo** | Reverts the last annotation step | `Ctrl + Z` |

---

## Actions & Exporting

The horizontal toolbar attached below your selection provides one-click output options:

- **📋 Copy to Clipboard (`Ctrl + C`)**: Copies the selected, annotated image directly to your system clipboard so you can paste (`Ctrl + V`) it into Slack, Discord, Telegram, or any document. Closes the overlay automatically.
- **💾 Save As (`Ctrl + S`)**: Opens a file dialog to choose the filename, folder, and format (`PNG`, `JPEG`, `BMP`).
- **☁ Upload (`Ctrl + D`)**: Uploads the screenshot to your configured image server and copies the resulting URL to your clipboard.
- **✕ Cancel (`Esc`)**: Aborts the capture and restores your desktop without saving.

---

## Preferences

Right-click the tray icon and select **Preferences...**:

### General Tab
- **Language**: Choose your preferred UI language.
- **Show Notifications**: Displays subtle desktop notifications when screenshots are copied or saved.
- **Capture Cursor**: Optionally include the mouse pointer in screenshots.
- **Keep Selected Area**: Remembers your previous selection position for rapid successive captures.

### Hotkeys Tab
- Configure custom key sequences for:
  - General region capture (Default: `Print Screen`)
  - Instant save fullscreen (Default: `Shift + Print Screen`)
  - Instant upload fullscreen (Default: Disabled)

### Formats Tab
- **Default Format**: PNG (lossless), JPEG (compressed), or BMP.
- **JPEG Quality**: Slider from 1% to 100% (Default: 90%).
- **Default Save Directory**: Folder where screenshots are saved by default.

### Upload & Network Tab
- Configure custom HTTP/HTTPS image upload servers.
- Configure proxy settings (Direct, System proxy, or Manual HTTP/HTTPS proxy).

---

## Troubleshooting

- **Wayland Environment**: On pure Wayland sessions, global key grabbing is restricted by design. You can assign a custom global shortcut in your desktop environment's settings (e.g. GNOME Settings -> Keyboard -> Custom Shortcuts) bound to `linuxshot --capture`.
- **System Tray Icon Missing**: On GNOME Shell, ensure an AppIndicator extension (such as `gnome-shell-extension-appindicator`) is enabled.

