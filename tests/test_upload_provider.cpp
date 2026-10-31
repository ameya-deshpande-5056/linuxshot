#include <QTest>
#include <QSignalSpy>
#include "upload/PrntscrProvider.h"
#include "upload/UploadManager.h"

class TestUploadProvider : public QObject {
    Q_OBJECT

private slots:
    void testTokenGeneration();
    void testHashDeterministic();
    void testUploadManagerToggle();
};

void TestUploadProvider::testTokenGeneration() {
    QString token = PrntscrProvider::generateToken();
    QCOMPARE(token, "37162342db68031b");
    QCOMPARE(token.length(), 16);
}

void TestUploadProvider::testHashDeterministic() {
    qint64 testTimestamp = 1730378400; // Fixed timestamp
    QString hash1 = PrntscrProvider::generateHash(testTimestamp);
    QString hash2 = PrntscrProvider::generateHash(testTimestamp);
    QCOMPARE(hash1, hash2);
    QCOMPARE(hash1, "d79d3cb2178650c314fd62886877e50f");
    QCOMPARE(hash1.length(), 32);

    // Verify format is lowercase hex
    for (QChar c : hash1) {
        bool isHex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
        QVERIFY(isHex);
    }
}

void TestUploadProvider::testUploadManagerToggle() {
    UploadManager manager;
    QCOMPARE(manager.prntscrEnabled(), false);

    manager.setPrntscrEnabled(true);
    QCOMPARE(manager.prntscrEnabled(), true);

    manager.setPrntscrEnabled(false);
    QCOMPARE(manager.prntscrEnabled(), false);
}

QTEST_MAIN(TestUploadProvider)
#include "test_upload_provider.moc"

