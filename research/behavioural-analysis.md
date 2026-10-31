# Observable Behavioural Analysis

## 1. Application Lifecycle & System Tray

1. **Startup**:
   - The application starts quietly in the background without opening a main window.
   - A feather icon appears in the system tray.
   - Global shortcuts are registered (Default: `Print Screen` for region capture).
   - Single-instance enforcement: launching a second instance either triggers a capture or focuses existing preferences.

2. **System Tray Interaction**:
   - **Left Click**: Triggers interactive region capture.
   - **Right Click**: Shows context menu:
     - "Take a screenshot" (triggers capture)
     - "Options..." (opens preferences dialog)
     - "About" (shows version and copyright)
     - "Exit" (quits application)

## 2. Capture Workflow

1. **Triggering**:
   - Triggered via global hotkey (`Print Screen`) or tray menu item.
   - Entire desktop across all monitors is captured in the background.

2. **Overlay Display**:
   - A full-screen window spans all connected monitors.
   - The captured desktop screenshot is displayed as the background.
   - A dark semi-transparent veil (50% black) dims the entire screen.
   - Mouse cursor becomes a crosshair (`Qt::CrossCursor`).
   - Instruction tooltip appears: "Select area".

3. **Selection Creation**:
   - Mouse press + drag defines the initial rectangular region.
   - Inside the selection rectangle, the dimmed veil is removed, showing the bright, un-dimmed screen content.
   - Dimension indicator appears showing `Width x Height` (e.g., `800 x 600 px`).

4. **Selection Adjustment**:
   - **8 Handles**: 4 corners and 4 edge centers have square resize handles.
   - Cursor changes appropriately:
     - Top-left / Bottom-right: diagonal resize (`SizeBDiagCursor`)
     - Top-right / Bottom-left: diagonal resize (`SizeFDiagCursor`)
     - Top / Bottom: vertical resize (`SizeVerCursor`)
     - Left / Right: horizontal resize (`SizeHorCursor`)
     - Inside selection: four-way move cursor (`SizeAllCursor`)
   - Dragging handles resizes the boundary with minimum dimensions (10x10 px).
   - Dragging inside moves the entire selection without resizing.
   - Arrow keys nudge selection by 1 pixel (Shift + Arrow keys by 10 pixels).
   - Pressing `Escape` cancels capture and returns to the desktop immediately.

## 3. Toolbars & Interaction

Once the selection is made, two floating toolbars attach to the selection rectangle:

### A. Vertical Annotation Toolbar (Attached to Right or Left Edge)
Contains 10 tool buttons (top to bottom):
1. **Pen** (freehand line drawing)
2. **Line** (straight line)
3. **Arrow** (line ending with an arrowhead)
4. **Rectangle** (hollow rectangle outline)
5. **Circle / Ellipse** (hollow ellipse outline)
6. **Marker** (semi-transparent highlighter with wider stroke)
7. **Text** (click to place text box, inline typing)
8. **Blur** (pixelation mosaic on drawn box)
9. **Color** (opens color palette / chooser)
10. **Undo** (reverts last annotation step)

### B. Horizontal Action Toolbar (Attached to Bottom or Top Edge)
Contains action buttons (left to right):
1. **Upload** (uploads to server, copies link)
2. **Share** (menu for Google search, social networks)
3. **Print** (sends to printer)
4. **Copy** (`Ctrl+C`, copies cropped image to system clipboard, closes overlay)
5. **Save** (`Ctrl+S`, opens Save As file dialog, saves image, closes overlay)
6. **Cancel / Close** (`Esc`, closes overlay without saving)

### Toolbar Auto-Repositioning
- If selection is too close to the screen bottom, the horizontal action toolbar flips above the selection.
- If selection is too close to the screen right, the vertical tool toolbar flips to the inside or left.

## 4. Annotation Tools Detail

- **Pen**: Continuous freehand path, smooth lines, respects active stroke width and color.
- **Line**: Straight line from start point to end point with live preview.
- **Arrow**: Straight line terminating in a solid filled triangular arrowhead.
- **Rectangle**: Hollow border with stroke width.
- **Circle**: Hollow ellipse with stroke width.
- **Marker**: Drawn with semi-transparent blending (`alpha = 80-100`) so underlying text remains readable.
- **Text**: Inline text entry. Click position places text origin. Text rendered with bold sans-serif font.
- **Blur**: Rectangular mosaic pixelation. Divides selected area into blocks (e.g. 10x10 px) and fills each block with the average color.
- **Undo**: Restores previous image state in reverse chronological order.
