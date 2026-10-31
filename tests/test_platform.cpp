#include <QTest>
#include "platform/Platform.h"

class TestPlatform : public QObject {
    Q_OBJECT

private slots:
    void testSessionType();
    void testWindowingSystem();
    void testDesktopEnvironment();
};

void TestPlatform::testSessionType() {
    QString session = Platform::sessionType();
    QVERIFY(!session.isEmpty());
}

void TestPlatform::testWindowingSystem() {
    Platform::WindowingSystem ws = Platform::windowingSystem();
    QVERIFY(ws == Platform::WindowingSystem::X11 || ws == Platform::WindowingSystem::Wayland || ws == Platform::WindowingSystem::Unknown);

    if (Platform::isX11()) {
        QCOMPARE(ws, Platform::WindowingSystem::X11);
        QVERIFY(!Platform::isWayland());
    } else if (Platform::isWayland()) {
        QCOMPARE(ws, Platform::WindowingSystem::Wayland);
        QVERIFY(!Platform::isX11());
    }
}

void TestPlatform::testDesktopEnvironment() {
    QString deName = Platform::desktopEnvironmentName();
    QVERIFY(!deName.isEmpty());
}

QTEST_MAIN(TestPlatform)
#include "test_platform.moc"
