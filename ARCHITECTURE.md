# LinuxShot Architecture

## Overview

LinuxShot is a Linux-native screenshot application implemented in C++20 with Qt 6.
It uses a modular architecture with platform abstraction to support both X11 and Wayland.

## Module Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         Application                             │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌───────────────────┐  │
│  │ Main     │ │ System   │ │ Settings │ │ Global Shortcuts  │  │
│  │ Window   │ │ Tray     │ │ Manager  │ │ Manager           │  │
│  └────┬─────┘ └──────────┘ └──────────┘ └───────────────────┘  │
│       │                                                         │
│  ┌────┴─────────────────────────────────────────────────────┐   │
│  │                   Capture Manager                         │   │
│  │  ┌────────────────┐  ┌────────────────────────────────┐  │   │
│  │  │ X11 Backend    │  │ Portal/PipeWire Backend        │  │   │
│  │  │ (XCB SHM)     │  │ (XDG Desktop Portal)           │  │   │
│  │  └────────────────┘  └────────────────────────────────┘  │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                Selection Overlay                          │   │
│  │  Full-screen transparent window with selection rectangle  │   │
│  │  Handles, dimensions, crosshair cursor                    │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                 Annotation Editor                         │   │
│  │  ┌─────────────────┐  ┌──────────────────────────────┐   │   │
│  │  │ Tool Manager    │  │ Command History (Undo/Redo)  │   │   │
│  │  │ - Pen           │  └──────────────────────────────┘   │   │
│  │  │ - Line          │                                     │   │
│  │  │ - Arrow         │  ┌──────────────────────────────┐   │   │
│  │  │ - Rectangle     │  │ Annotation Renderer          │   │   │
│  │  │ - Circle        │  │ (QPainter-based)             │   │   │
│  │  │ - Marker        │  └──────────────────────────────┘   │   │
│  │  │ - Text          │                                     │   │
│  │  │ - Blur          │  ┌──────────────────────────────┐   │   │
│  │  └─────────────────┘  │ Toolbar (QML)               │   │   │
│  │                       └──────────────────────────────┘   │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                 Export Manager                             │   │
│  │  Save | Clipboard | Upload | Print                        │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                 Upload System                             │   │
│  │  ┌─────────────────┐  ┌──────────────────────────────┐   │   │
│  │  │ Provider I/F    │  │ Self-hosted Provider         │   │   │
│  │  └─────────────────┘  └──────────────────────────────┘   │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

## Key Design Decisions

### 1. Capture Pipeline

**X11**: Use XCB with SHM extension for zero-copy screen capture.
`xcb_shm_get_image()` captures the root window, then we convert to QImage.

**Wayland**: Use XDG Desktop Portal's `org.freedesktop.portal.Screenshot`
interface via D-Bus. This respects compositor security — user gets a permission
dialog. For screen recording/streaming, use the ScreenCast portal + PipeWire.

### 2. Selection Overlay

A frameless, transparent, fullscreen window displayed on top of all windows.
On X11, use `_NET_WM_WINDOW_TYPE_DOCK` or override-redirect.
The captured screenshot is displayed as background, with a darkened overlay
and a bright selection rectangle.

### 3. Annotation Engine

Command pattern for undo/redo:
- Each annotation operation (draw line, add text, etc.) is an `AnnotationCommand`
- Commands stored in a `CommandHistory` stack
- Undo pops and reverses; Redo replays
- Original screenshot preserved — annotations rendered as overlay

### 4. Toolbar

Two compact toolbars attached to the selection rectangle:
- **Left/bottom**: Annotation tools (pen, line, arrow, rect, circle, marker, text, blur, color, undo)
- **Right/top**: Actions (save, copy, upload, print, close)

Positioned automatically to avoid going off-screen.

### 5. Settings

Stored in `~/.config/linuxshot/linuxshot.conf` (QSettings with INI format).
XDG Base Directory compliant.

### 6. Platform Abstraction

```cpp
class CaptureBackend {
public:
    virtual ~CaptureBackend() = default;
    virtual QImage captureScreen(int screenIndex = -1) = 0;
    virtual QImage captureFullDesktop() = 0;
    virtual QList<QRect> screenGeometries() = 0;
};

class X11CaptureBackend : public CaptureBackend { ... };
class PortalCaptureBackend : public CaptureBackend { ... };
```

### 7. Upload System

```cpp
class UploadProvider {
public:
    virtual ~UploadProvider() = default;
    virtual QString name() const = 0;
    virtual QFuture<UploadResult> upload(const QImage &image, const UploadOptions &opts) = 0;
    virtual bool isConfigured() const = 0;
};
```

## File Structure

```
src/
├── main.cpp
├── core/
│   └── Application.h/cpp         # App lifecycle, single instance
├── settings/
│   └── SettingsManager.h/cpp     # XDG-compliant configuration persistence
├── platform/
│   ├── Platform.h/cpp            # Platform/session/desktop environment abstraction
│   └── GlobalShortcutManager.h/cpp # XCB global hotkey grabber & event filter
├── capture/
│   ├── CaptureBackend.h          # Abstract capture interface
│   ├── CaptureManager.h/cpp      # Backend selection & coordination
│   ├── X11CaptureBackend.h/cpp   # XCB SHM capture
│   └── PortalCaptureBackend.h/cpp # XDG Desktop Portal capture
├── ui/
│   ├── SelectionOverlay.h/cpp    # Fullscreen selection window
│   ├── AnnotationEditor.h/cpp    # Drawing canvas
│   ├── Toolbar.h/cpp             # Floating tool and action toolbars
│   ├── ColorButton.h/cpp         # Color picker swatch button
│   ├── TrayIcon.h/cpp            # System tray integration
│   └── SettingsDialog.h/cpp      # Tabbed preferences dialog
│   ├── TrayIcon.h/cpp            # System tray integration
│   └── SettingsDialog.h/cpp      # Settings window
├── annotation/
│   ├── AnnotationTool.h          # Abstract tool interface
│   ├── PenTool.h/cpp
│   ├── LineTool.h/cpp
│   ├── ArrowTool.h/cpp
│   ├── RectTool.h/cpp
│   ├── CircleTool.h/cpp
│   ├── MarkerTool.h/cpp
│   ├── TextTool.h/cpp
│   ├── BlurTool.h/cpp
│   ├── AnnotationCommand.h       # Command pattern base
│   └── CommandHistory.h/cpp      # Undo/redo stack
├── export/
│   ├── ExportManager.h/cpp       # Save, clipboard, print coordination
│   └── ImageExporter.h/cpp       # Format-specific export
└── upload/
    ├── UploadProvider.h           # Abstract upload interface
    ├── UploadManager.h/cpp        # Upload coordination
    └── SelfHostedProvider.h/cpp   # Example self-hosted backend
```

## Dependencies

| Dependency | Purpose | Required |
|-----------|---------|----------|
| Qt 6 Core | Foundation | Yes |
| Qt 6 Gui | Image handling, painting | Yes |
| Qt 6 Widgets | Dialogs, tray, menus | Yes |
| Qt 6 Network | Upload HTTP client | Yes |
| Qt 6 DBus | Portal communication | Yes (Wayland) |
| XCB | X11 screen capture | Yes (X11) |
| XCB-SHM | Zero-copy capture | Yes (X11) |
| XCB-RANDR | Multi-monitor info | Yes (X11) |
| XCB-KEYSYMS | Global shortcuts | Yes (X11) |

## Build Requirements

- CMake 3.22+
- GCC 13+ or Clang 16+ (C++20)
- Qt 6.4+
- XCB development libraries
- pkg-config

