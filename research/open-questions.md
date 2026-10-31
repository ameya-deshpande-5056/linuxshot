# Open Questions and Hypotheses

## 1. Wayland Global Shortcut Support

- **Status**: Confirmed limitation on pure Wayland.
- **Finding**: Unlike X11 where `xcb_grab_key` allows any application to register a global key combination, Wayland compositors prohibit direct key grabbing for security reasons.
- **Strategy**:
  1. On X11: use native XCB global key grabbing (`xcb_grab_key`).
  2. On Wayland: leverage `org.freedesktop.portal.GlobalShortcuts` when supported by the compositor, provide standard system shortcut recommendations (e.g., binding `linuxshot --capture` in desktop settings), and provide a system tray menu click.

## 2. Image Hosting Backend

- **Status**: Confirmed legal & architectural separation.
- **Finding**: Lightshot's proprietary server (`https://upload.prntscr.com/upload`) requires client identification / headers and generates public shortlinks on `prnt.sc`. Direct reproduction of their server violates the clean-room specification.
- **Strategy**: Implement an open, pluggable upload architecture:
  1. Built-in configurable HTTP/HTTPS multipart upload provider (compatible with standard image hosting endpoints, Imgur, or self-hosted instances).
  2. Documented specification and test server for self-hosting.
  3. Safe fallback when offline or unconfigured.

## 3. High-DPI Scaling & Device Pixel Ratio

- **Status**: Confirmed behavior.
- **Finding**: On high-DPI screens, desktop coordinates may differ from physical pixel dimensions.
- **Strategy**: Qt 6 has native High-DPI scaling (`devicePixelRatio()`). Coordinates during selection must map accurately to underlying screenshot pixel buffers so that exported images remain crisp and 1:1 with screen pixels.

