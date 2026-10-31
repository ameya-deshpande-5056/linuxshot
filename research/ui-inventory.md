# UI Element Inventory

Comprehensive inventory of all observable UI widgets, controls, and dialogs.

## 1. System Tray
- Icon: Feather silhouette.
- Tooltip: `Press "<hotkey>" on your keyboard to take a screenshot`.
- Context menu:
  - `Take a screenshot`
  - Separator
  - `Options...`
  - `About`
  - Separator
  - `Exit`

## 2. Selection Overlay
- Fullscreen borderless window spanning virtual desktop geometry.
- Dark semi-transparent scrim (`#000000` with 50% opacity).
- Clear rectangular cutout displaying the unshaded background image.
- Dashed selection boundary line (1px, high-contrast cyan/blue `#0084FF`).
- 8 resize handles (6x6 px filled squares at vertices and edge centers).
- Tooltip label displaying selection dimensions (e.g., `640 x 480`).

## 3. Annotation Toolbar (Vertical)
Compact floating toolbar with 28x28 px tool buttons and dark styling (`#2D2D2D` background):
1. **Pen Button**: Icon of a pencil. Checkable.
2. **Line Button**: Icon of a diagonal line. Checkable.
3. **Arrow Button**: Icon of an arrow. Checkable.
4. **Rectangle Button**: Icon of a hollow box. Checkable.
5. **Circle Button**: Icon of a hollow circle. Checkable.
6. **Marker Button**: Icon of a highlighter pen. Checkable.
7. **Text Button**: Icon of the letter 'T'. Checkable.
8. **Blur Button**: Icon of a checkered / pixelated tile. Checkable.
9. **Color Picker Button**: Shows active color circle swatch; opens palette on click.
10. **Undo Button**: Icon of a counter-clockwise curved arrow. Enabled when undo stack is not empty.

## 4. Action Toolbar (Horizontal)
Compact floating toolbar alongside selection bottom:
1. **Upload Button**: Cloud upload icon.
2. **Share Button**: Share / link icon.
3. **Print Button**: Printer icon.
4. **Copy Button**: Two overlapping documents icon.
5. **Save Button**: Floppy disk / save icon.
6. **Close / Cancel Button**: 'X' icon.

## 5. Options Dialog (Tabbed / Property Sheet)
- **General Tab**:
  - Automatically copy link after uploading (Checkbox, default: true).
  - Automatically close the upload window (Checkbox, default: true).
  - Show notifications about copying and saving (Checkbox, default: true).
  - Capture cursor on screenshot (Checkbox, default: false).
  - Keep the selected area position (Checkbox, default: false).
  - Language dropdown (ComboBox).
- **Hotkeys Tab**:
  - General hotkey: Checkbox + Key sequence input (Default: `Print`).
  - Instant save fullscreen: Checkbox + Key sequence input (Default: `Shift+Print`).
  - Instant upload fullscreen: Checkbox + Key sequence input (Default: Disabled).
- **Formats Tab**:
  - Save format dropdown (`PNG`, `JPEG`, `BMP`).
  - JPEG quality slider: `1` to `100` (Default: `90`).
  - Default save folder path + Browse button.
- **Upload / Network Tab**:
  - Proxy configuration: No proxy, System proxy, Manual proxy.
  - Upload provider / endpoint URL.
