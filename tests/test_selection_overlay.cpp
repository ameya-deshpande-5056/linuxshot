#include <QTest>
#include <QImage>
#include "ui/SelectionOverlay.h"
#include "ui/AnnotationEditor.h"

class TestSelectionOverlay : public QObject {
    Q_OBJECT

private slots:
    void testHighDpiFullScreenCrop();
    void testHighDpiSubRegionCrop();
    void testAnnotationEditorRenderingScale();
};

void TestSelectionOverlay::testHighDpiFullScreenCrop() {
    // 1920x1080 physical desktop screenshot
    QImage desktop(1920, 1080, QImage::Format_RGB32);
    desktop.fill(Qt::black);
    // Mark top-left and bottom-right pixels
    desktop.setPixelColor(0, 0, Qt::red);
    desktop.setPixelColor(1919, 1079, Qt::green);

    SelectionOverlay overlay;
    overlay.show();
    overlay.setScreenshot(desktop);

    // Simulate 125% display zoom: logical size 1536x864
    overlay.resize(1536, 864);

    // Select the entire visible screen area in logical coordinates
    overlay.setSelectionRect(QRect(0, 0, 1536, 864));

    QVERIFY(overlay.editor() != nullptr);
    QVERIFY(!overlay.editor()->isHidden());

    QImage cropped = overlay.editor()->getRenderedImage();
    QCOMPARE(cropped.size(), QSize(1920, 1080));

    // Verify both corners are captured without truncation
    QCOMPARE(cropped.pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(cropped.pixelColor(1919, 1079), QColor(Qt::green));
}

void TestSelectionOverlay::testHighDpiSubRegionCrop() {
    // 1920x1080 physical desktop screenshot
    QImage desktop(1920, 1080, QImage::Format_RGB32);
    desktop.fill(Qt::black);
    desktop.setPixelColor(1919, 1079, Qt::cyan);

    SelectionOverlay overlay;
    overlay.setScreenshot(desktop);

    // Simulate 200% display zoom: logical size 960x540
    overlay.resize(960, 540);

    // Select the bottom-right quadrant: logical (480, 270) to (960, 540)
    overlay.setSelectionRect(QRect(480, 270, 480, 270));

    QVERIFY(overlay.editor() != nullptr);
    QImage cropped = overlay.editor()->getRenderedImage();

    // Physical size of bottom-right quadrant should be 960x540
    QCOMPARE(cropped.size(), QSize(960, 540));

    // The bottom-right pixel of the cropped image should be the cyan marker
    QCOMPARE(cropped.pixelColor(959, 539), QColor(Qt::cyan));
}

void TestSelectionOverlay::testAnnotationEditorRenderingScale() {
    AnnotationEditor editor;
    
    // Simulate high-DPI base image (500x375 device pixels) for a 400x300 logical widget
    QImage base(500, 375, QImage::Format_RGB32);
    base.fill(Qt::white);

    editor.resize(400, 300);
    editor.setBaseImage(base);

    QCOMPARE(editor.size(), QSize(400, 300));

    QImage rendered = editor.getRenderedImage();
    QCOMPARE(rendered.size(), QSize(500, 375));
}

QTEST_MAIN(TestSelectionOverlay)
#include "test_selection_overlay.moc"
