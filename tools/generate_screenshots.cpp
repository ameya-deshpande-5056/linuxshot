#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QFontMetrics>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <iostream>

#include "ui/SelectionOverlay.h"
#include "ui/AnnotationEditor.h"
#include "ui/SettingsDialog.h"
#include "ui/AboutDialog.h"
#include "settings/SettingsManager.h"
#include "annotation/RectTool.h"
#include "annotation/ArrowTool.h"
#include "annotation/CircleTool.h"
#include "annotation/MarkerTool.h"
#include "annotation/BlurTool.h"
#include "annotation/TextTool.h"
#include "annotation/PenTool.h"
#include "annotation/LineTool.h"

static QImage createMockDesktop(int w, int h) {
    QImage desktop(w, h, QImage::Format_ARGB32_Premultiplied);
    desktop.fill(QColor("#181926")); // Dark modern background

    QPainter p(&desktop);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    // Subtle desktop gradient wallpaper
    QLinearGradient bgGrad(0, 0, w, h);
    bgGrad.setColorAt(0.0, QColor("#1e1e2e"));
    bgGrad.setColorAt(0.5, QColor("#181825"));
    bgGrad.setColorAt(1.0, QColor("#11111b"));
    p.fillRect(0, 0, w, h, bgGrad);

    // Ambient wallpaper decorative glow
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(137, 180, 250, 25));
    p.drawEllipse(QPointF(w * 0.25, h * 0.3), 350, 350);
    p.setBrush(QColor(203, 166, 247, 22));
    p.drawEllipse(QPointF(w * 0.75, h * 0.7), 400, 400);

    // Top Linux desktop panel (GNOME/Cinnamon style)
    p.fillRect(0, 0, w, 32, QColor(24, 24, 37, 240));
    p.setPen(QColor("#cdd6f4"));
    QFont panelFont("Sans Serif", 10, QFont::Medium);
    p.setFont(panelFont);
    p.drawText(20, 21, "Applications");
    p.drawText(w / 2 - 40, 21, "Sat 12:30 PM");
    p.drawText(w - 180, 21, "Vol  |  Wi-Fi  |  98%");

    // Mock IDE / Code Editor Window in center
    QRect winRect(260, 110, 1020, 680);

    // Window shadow
    for (int i = 1; i <= 8; ++i) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 16 - i * 2));
        p.drawRoundedRect(winRect.adjusted(-i * 2, -i * 2, i * 2, i * 2 + 4), 12 + i, 12 + i);
    }

    // Window base
    p.setBrush(QColor("#1e1e2e"));
    p.setPen(QColor("#313244"));
    p.drawRoundedRect(winRect, 10, 10);

    // Window title bar
    QRect titleBar(winRect.x(), winRect.y(), winRect.width(), 38);
    QPainterPath titlePath;
    titlePath.addRoundedRect(titleBar, 10, 10);
    p.fillPath(titlePath, QColor("#181825"));
    p.fillRect(titleBar.x(), titleBar.bottom() - 10, titleBar.width(), 11, QColor("#181825")); // square off bottom

    // Window buttons (red, yellow, green)
    p.setBrush(QColor("#f38ba8")); p.setPen(Qt::NoPen);
    p.drawEllipse(winRect.x() + 16, winRect.y() + 13, 12, 12);
    p.setBrush(QColor("#f9e2af"));
    p.drawEllipse(winRect.x() + 36, winRect.y() + 13, 12, 12);
    p.setBrush(QColor("#a6e3a1"));
    p.drawEllipse(winRect.x() + 56, winRect.y() + 13, 12, 12);

    // Title text
    p.setFont(QFont("Sans Serif", 10, QFont::Bold));
    p.setPen(QColor("#a6adc8"));
    p.drawText(titleBar, Qt::AlignCenter, "Application.cpp — LinuxShot (C++20 / Qt 6)");

    // Window Sidebar (File tree)
    QRect sideBar(winRect.x(), titleBar.bottom() + 1, 200, winRect.height() - titleBar.height() - 25);
    p.fillRect(sideBar, QColor("#11111b"));
    p.setPen(QColor("#313244"));
    p.drawLine(sideBar.topRight(), sideBar.bottomRight());

    p.setFont(QFont("Monospace", 9));
    p.setPen(QColor("#6c7086"));
    p.drawText(sideBar.x() + 14, sideBar.y() + 24, "PROJECT EXPLORER");
    p.setPen(QColor("#cdd6f4"));
    p.drawText(sideBar.x() + 20, sideBar.y() + 50, "v src/");
    p.drawText(sideBar.x() + 34, sideBar.y() + 72, "  main.cpp");
    p.setPen(QColor("#89b4fa"));
    p.drawText(sideBar.x() + 34, sideBar.y() + 94, "> Application.cpp");
    p.setPen(QColor("#cdd6f4"));
    p.drawText(sideBar.x() + 34, sideBar.y() + 116, "  CaptureBackend.h");
    p.drawText(sideBar.x() + 34, sideBar.y() + 138, "  SelectionOverlay.cpp");
    p.drawText(sideBar.x() + 34, sideBar.y() + 160, "  AnnotationEditor.cpp");
    p.drawText(sideBar.x() + 34, sideBar.y() + 182, "  Toolbar.cpp");

    // Code Editor area
    QRect editorArea(sideBar.right() + 1, titleBar.bottom() + 1, winRect.width() - sideBar.width() - 1, sideBar.height());
    p.fillRect(editorArea, QColor("#1e1e2e"));

    QFont codeFont("DejaVu Sans Mono", 10);
    p.setFont(codeFont);

    const char* lines[] = {
        "// LinuxShot: Dual-stack X11 zero-copy XCB-SHM & Wayland Portal capture",
        "void Application::captureRegion() {",
        "    const auto desktop = m_captureManager->grabDesktop();",
        "    m_overlay->setScreenshot(desktop);",
        "    m_overlay->show();",
        "}",
        "",
        "// Vector annotation engine with unlimited undo/redo stack",
        "bool Application::exportCapture(const QString &savePath) {",
        "    const QImage rendered = m_overlay->editor()->getRenderedImage();",
        "    const QString token = \"SEC_AUTH_KEY_9841f3e2\"; // internal secret",
        "    if (m_exporter->saveToFile(rendered, savePath, Format::PNG)) {",
        "        m_trayIcon->showMessage(\"Saved\", \"Screenshot copied to clipboard\");",
        "        return true;",
        "    }",
        "    return false;",
        "}"
    };

    int startY = editorArea.y() + 32;
    for (int i = 0; i < 17; ++i) {
        int y = startY + i * 22;

        // Line numbers
        p.setPen(QColor("#585b70"));
        p.drawText(editorArea.x() + 12, y, QString("%1").arg(i + 1, 2));

        // Code text with custom coloring
        QString str = lines[i];
        if (str.startsWith("//")) {
            p.setPen(QColor("#6c7086")); // Comment
        } else if (str.contains("void") || str.contains("bool") || str.contains("const") || str.contains("return")) {
            p.setPen(QColor("#cba6f7")); // Keyword
        } else if (str.contains("\"")) {
            p.setPen(QColor("#a6e3a1")); // String
        } else {
            p.setPen(QColor("#cdd6f4")); // Default text
        }
        p.drawText(editorArea.x() + 48, y, str);
    }

    // Bottom status bar
    QRect statusBar(winRect.x(), editorArea.bottom(), winRect.width(), 24);
    p.fillRect(statusBar, QColor("#181825"));
    p.setPen(QColor("#6c7086"));
    p.setFont(QFont("Sans Serif", 9));
    p.drawText(statusBar.x() + 16, statusBar.y() + 16, "UTF-8   |   C++20   |   Linux (XCB-SHM)   |   Git: master*   |   LinuxShot v0.1.0");

    p.end();
    return desktop;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("LinuxShot");
    app.setApplicationDisplayName("LinuxShot");
    app.setApplicationVersion(LINUXSHOT_VERSION);
    app.setWindowIcon(QIcon(":/icons/linuxshot.png"));

    QDir().mkpath("docs/screenshots");

    const int screenW = 1920;
    const int screenH = 1080;
    QImage desktop = createMockDesktop(screenW, screenH);

    // =========================================================================
    // 1. SCENARIO 1: Region Selection & Floating Toolbars
    // =========================================================================
    {
        SelectionOverlay overlay;
        overlay.resize(screenW, screenH);
        overlay.setScreenshot(desktop);

        // Region over code editor
        QRect captureRect(320, 160, 880, 520);
        overlay.setSelectionRect(captureRect);
        overlay.show();
        QApplication::processEvents();

        // Render full widget onto pixmap
        QPixmap fullCanvas(screenW, screenH);
        overlay.render(&fullCanvas);

        // Crop nicely framed around the window and floating toolbars
        QRect frameRect(160, 60, 1280, 760);
        QImage showcase1 = fullCanvas.copy(frameRect).toImage();
        showcase1.save("docs/screenshots/01-region-selection.png", "PNG");
        std::cout << "Saved: docs/screenshots/01-region-selection.png ("
                  << showcase1.width() << "x" << showcase1.height() << ")" << std::endl;
        overlay.hide();
    }

    // =========================================================================
    // 2. SCENARIO 2: Interactive Vector Annotations (All 8 Tools)
    // =========================================================================
    {
        SelectionOverlay overlay;
        overlay.resize(screenW, screenH);
        overlay.setScreenshot(desktop);

        QRect captureRect(320, 160, 880, 520);
        overlay.setSelectionRect(captureRect);
        overlay.show();
        QApplication::processEvents();

        AnnotationEditor *editor = overlay.editor();

        // 1. Red Rectangle around captureRegion method
        editor->addCommand(std::make_unique<RectCommand>(
            QRectF(175, 30, 460, 94),
            QColor("#FF3B30"), 3
        ));

        // 2. Blue Arrow pointing to grabDesktop()
        editor->addCommand(std::make_unique<ArrowCommand>(
            QPointF(670, 75),
            QPointF(535, 75),
            QColor("#007AFF"), 3
        ));

        // 3. Semi-transparent Yellow Marker highlighting setScreenshot line
        QPolygonF markerPoints;
        markerPoints << QPointF(190, 97) << QPointF(470, 97);
        editor->addCommand(std::make_unique<MarkerCommand>(
            markerPoints,
            QColor(255, 214, 10, 110), 16
        ));

        // 4. Privacy Blur / Pixelate over sensitive key "SEC_AUTH_KEY_9841f3e2"
        auto blurCmd = std::make_unique<BlurCommand>(QRectF(380, 240, 210, 24), 8);
        blurCmd->cachePixelation(editor->baseImage());
        editor->addCommand(std::move(blurCmd));

        // 5. Green Circle highlighting return true
        editor->addCommand(std::make_unique<CircleCommand>(
            QRectF(190, 305, 120, 28),
            QColor("#34C759"), 3
        ));

        // 6. Straight Line under the vector annotation comment
        editor->addCommand(std::make_unique<LineCommand>(
            QPointF(190, 185),
            QPointF(580, 185),
            QColor("#BF5AF2"), 2
        ));

        // 7. Freehand Pen Stroke checkmark
        QPolygonF penPoints;
        penPoints << QPointF(650, 68) << QPointF(658, 78) << QPointF(675, 58);
        editor->addCommand(std::make_unique<PenCommand>(
            penPoints,
            QColor("#30D158"), 3
        ));

        // 8. Text Callout Badge
        QFont textFont("Sans Serif", 11, QFont::Bold);
        editor->addCommand(std::make_unique<TextCommand>(
            "Zero-copy XCB-SHM screen capture",
            QPointF(420, 140),
            textFont,
            QColor("#FFFFFF")
        ));

        QApplication::processEvents();

        // Render full canvas and crop
        QPixmap fullCanvas(screenW, screenH);
        overlay.render(&fullCanvas);

        QRect frameRect(160, 60, 1280, 760);
        QImage showcase2 = fullCanvas.copy(frameRect).toImage();
        showcase2.save("docs/screenshots/02-annotations-showcase.png", "PNG");
        std::cout << "Saved: docs/screenshots/02-annotations-showcase.png ("
                  << showcase2.width() << "x" << showcase2.height() << ")" << std::endl;
        overlay.hide();
    }

    // =========================================================================
    // 3. SCENARIO 3: Floating Toolbars & Color Picker Close-up
    // =========================================================================
    {
        SelectionOverlay overlay;
        overlay.resize(screenW, screenH);
        overlay.setScreenshot(desktop);

        QRect captureRect(320, 160, 880, 520);
        overlay.setSelectionRect(captureRect);
        overlay.show();
        QApplication::processEvents();

        QPixmap fullCanvas(screenW, screenH);
        overlay.render(&fullCanvas);

        // Frame region encompassing the bottom-right of selection and both toolbars
        QRect toolbarsUnion = overlay.toolBar()->geometry().united(overlay.actionBar()->geometry());
        QRect toolbarFrame = toolbarsUnion.adjusted(-24, -24, 24, 24);
        QImage showcase3 = fullCanvas.copy(toolbarFrame).toImage();
        showcase3.save("docs/screenshots/03-toolbars-palette.png", "PNG");
        std::cout << "Saved: docs/screenshots/03-toolbars-palette.png ("
                  << showcase3.width() << "x" << showcase3.height() << ")" << std::endl;
        overlay.hide();
    }

    // =========================================================================
    // 4. SCENARIO 4: Preferences & Settings Dialog
    // =========================================================================
    {
        SettingsManager settings;
        SettingsDialog dialog(&settings);
        dialog.resize(520, 450);
        dialog.show();
        QApplication::processEvents();

        QPixmap dialogPix = dialog.grab();
        dialogPix.toImage().save("docs/screenshots/04-preferences.png", "PNG");
        std::cout << "Saved: docs/screenshots/04-preferences.png ("
                  << dialogPix.width() << "x" << dialogPix.height() << ")" << std::endl;
        dialog.hide();
    }

    // =========================================================================
    // 5. SCENARIO 5: About LinuxShot Dialog
    // =========================================================================
    {
        AboutDialog dialog;
        dialog.show();
        QApplication::processEvents();

        QPixmap dialogPix = dialog.grab();
        dialogPix.toImage().save("docs/screenshots/05-about-dialog.png", "PNG");
        std::cout << "Saved: docs/screenshots/05-about-dialog.png ("
                  << dialogPix.width() << "x" << dialogPix.height() << ")" << std::endl;
        dialog.hide();
    }

    std::cout << "All scenario screenshots successfully generated in docs/screenshots/" << std::endl;
    return 0;
}
