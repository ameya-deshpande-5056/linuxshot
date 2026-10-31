#include <QTest>
#include <QIcon>
#include "ui/IconHelper.h"

class TestIcons : public QObject {
    Q_OBJECT

private slots:
    void testActionIcons();
    void testToolIcons();
    void testIconPixmaps();
    void testRedesignedIcons();
    void testIconTinting();
};

static bool hasVisiblePixels(const QImage &img) {
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            if (qAlpha(img.pixel(x, y)) > 0) return true;
        }
    }
    return false;
}

void TestIcons::testRedesignedIcons() {
    // 1. Text icon
    QIcon textIcon = IconHelper::toolIcon(ToolType::Text);
    QVERIFY(!textIcon.isNull());
    QImage textImg = textIcon.pixmap(32, 32).toImage();
    QCOMPARE(textImg.size(), QSize(32, 32));
    QVERIFY(hasVisiblePixels(textImg));

    // 2. Undo icon
    QIcon undoIcon = IconHelper::actionIcon(IconHelper::Action::Undo);
    QVERIFY(!undoIcon.isNull());
    QImage undoImg = undoIcon.pixmap(32, 32).toImage();
    QCOMPARE(undoImg.size(), QSize(32, 32));
    QVERIFY(hasVisiblePixels(undoImg));

    // 3. Upload icon
    QIcon uploadIcon = IconHelper::actionIcon(IconHelper::Action::Upload);
    QVERIFY(!uploadIcon.isNull());
    QImage uploadImg = uploadIcon.pixmap(32, 32).toImage();
    QCOMPARE(uploadImg.size(), QSize(32, 32));
    QVERIFY(hasVisiblePixels(uploadImg));
}

void TestIcons::testActionIcons() {
    IconHelper::Action actions[] = {
        IconHelper::Action::Save,
        IconHelper::Action::Copy,
        IconHelper::Action::Upload,
        IconHelper::Action::Cancel,
        IconHelper::Action::Undo
    };

    for (auto action : actions) {
        QIcon icon = IconHelper::actionIcon(action);
        QVERIFY(!icon.isNull());
        QPixmap px = icon.pixmap(24, 24);
        QVERIFY(!px.isNull());
        QVERIFY(px.width() > 0);
        QVERIFY(px.height() > 0);
    }
}

void TestIcons::testToolIcons() {
    ToolType tools[] = {
        ToolType::Pen,
        ToolType::Line,
        ToolType::Arrow,
        ToolType::Rectangle,
        ToolType::Circle,
        ToolType::Marker,
        ToolType::Text,
        ToolType::Blur
    };

    for (auto tool : tools) {
        QIcon icon = IconHelper::toolIcon(tool);
        QVERIFY(!icon.isNull());
        QPixmap px = icon.pixmap(24, 24);
        QVERIFY(!px.isNull());
        QVERIFY(px.width() > 0);
        QVERIFY(px.height() > 0);
    }
}

void TestIcons::testIconPixmaps() {
    QIcon saveIcon = IconHelper::actionIcon(IconHelper::Action::Save);
    QImage img = saveIcon.pixmap(32, 32).toImage();
    
    // Verify that the icon is not just an empty transparent pixmap
    bool hasNonTransparentPixels = false;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            if (qAlpha(img.pixel(x, y)) > 0) {
                hasNonTransparentPixels = true;
                break;
            }
        }
        if (hasNonTransparentPixels) break;
    }
    QVERIFY(hasNonTransparentPixels);
}

void TestIcons::testIconTinting() {
    QColor tintColor("#E2E8F0");
    QIcon blurIcon = IconHelper::toolIcon(ToolType::Blur, tintColor);
    QVERIFY(!blurIcon.isNull());
    QImage img = blurIcon.pixmap(32, 32).toImage();
    QVERIFY(!img.isNull());

    bool foundTintedPixel = false;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QRgb rgb = img.pixel(x, y);
            if (qAlpha(rgb) > 200) {
                // Must not be black; should be light/white (e.g. red, green, blue > 180)
                if (qRed(rgb) > 180 && qGreen(rgb) > 180 && qBlue(rgb) > 180) {
                    foundTintedPixel = true;
                    break;
                }
            }
        }
        if (foundTintedPixel) break;
    }
    QVERIFY2(foundTintedPixel, "Blur icon was not properly tinted to light/white color in dark mode!");
}

QTEST_MAIN(TestIcons)
#include "test_icons.moc"
