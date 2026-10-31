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
};

void TestIcons::testRedesignedIcons() {
    // 1. Text icon
    QIcon textIcon = IconHelper::toolIcon(ToolType::Text);
    QVERIFY(!textIcon.isNull());
    QImage textImg = textIcon.pixmap(32, 32).toImage();
    QCOMPARE(textImg.size(), QSize(32, 32));
    // Verify pixels exist in center (letter T stem)
    QVERIFY(qAlpha(textImg.pixel(16, 16)) > 0);

    // 2. Undo icon
    QIcon undoIcon = IconHelper::actionIcon(IconHelper::Action::Undo);
    QVERIFY(!undoIcon.isNull());
    QImage undoImg = undoIcon.pixmap(32, 32).toImage();
    QCOMPARE(undoImg.size(), QSize(32, 32));
    // Verify arrowhead tip has non-transparent pixels
    QVERIFY(qAlpha(undoImg.pixel(6, 16)) > 0);

    // 3. Upload icon
    QIcon uploadIcon = IconHelper::actionIcon(IconHelper::Action::Upload);
    QVERIFY(!uploadIcon.isNull());
    QImage uploadImg = uploadIcon.pixmap(32, 32).toImage();
    QCOMPARE(uploadImg.size(), QSize(32, 32));
    // Verify arrow stem has non-transparent pixels
    QVERIFY(qAlpha(uploadImg.pixel(16, 20)) > 0);
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

QTEST_MAIN(TestIcons)
#include "test_icons.moc"
