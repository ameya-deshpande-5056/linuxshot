#include <QTest>
#include <QTemporaryDir>
#include "settings/SettingsManager.h"

class TestSettings : public QObject {
    Q_OBJECT

private slots:
    void testDefaults();
    void testSetters();
    void testQualityBounds();
};

void TestSettings::testDefaults() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    QString testConf = tempDir.filePath("test_defaults.conf");

    SettingsManager settings(testConf);
    QCOMPARE(settings.generalHotkey(), "Print");
    QCOMPARE(settings.generalHotkeyEnabled(), true);
    QCOMPARE(settings.saveFullscreenHotkey(), "Shift+Print");
    QCOMPARE(settings.defaultFormat(), "png");
    QCOMPARE(settings.jpegQuality(), 90);
    QCOMPARE(settings.showNotifications(), true);
    QCOMPARE(settings.captureCursor(), false);
    QCOMPARE(settings.keepSelection(), false);
}

void TestSettings::testSetters() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    QString testConf = tempDir.filePath("test_setters.conf");

    SettingsManager settings(testConf);
    settings.setGeneralHotkey("Ctrl+Shift+S");
    QCOMPARE(settings.generalHotkey(), "Ctrl+Shift+S");

    settings.setDefaultFormat("jpg");
    QCOMPARE(settings.defaultFormat(), "jpg");

    settings.setShowNotifications(false);
    QCOMPARE(settings.showNotifications(), false);

    settings.setPenColor(Qt::blue);
    QCOMPARE(settings.penColor(), QColor(Qt::blue));

    settings.setPenWidth(8);
    QCOMPARE(settings.penWidth(), 8);
}

void TestSettings::testQualityBounds() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    QString testConf = tempDir.filePath("test_bounds.conf");

    SettingsManager settings(testConf);
    settings.setJpegQuality(150); // should clamp to 100
    QCOMPARE(settings.jpegQuality(), 100);

    settings.setJpegQuality(-10); // should clamp to 1
    QCOMPARE(settings.jpegQuality(), 1);

    settings.setJpegQuality(85);
    QCOMPARE(settings.jpegQuality(), 85);
}

QTEST_MAIN(TestSettings)
#include "test_settings.moc"
