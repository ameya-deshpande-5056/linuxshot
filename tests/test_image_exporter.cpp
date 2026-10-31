#include <QTest>
#include <QTemporaryDir>
#include "export/ImageExporter.h"

class TestImageExporter : public QObject {
    Q_OBJECT

private slots:
    void testExportPNG();
    void testExportJPEG();
    void testExportBMP();
    void testNullImageFails();
    void testDefaultFilename();
};

void TestImageExporter::testExportPNG() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::red);

    QString path = dir.filePath("test.png");
    QVERIFY(ImageExporter::exportPNG(img, path));
    QVERIFY(QFile::exists(path));

    QImage loaded(path);
    QCOMPARE(loaded.size(), QSize(100, 100));
}

void TestImageExporter::testExportJPEG() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QImage img(120, 80, QImage::Format_RGB32);
    img.fill(Qt::green);

    QString path = dir.filePath("test.jpg");
    QVERIFY(ImageExporter::exportJPEG(img, path, 95));
    QVERIFY(QFile::exists(path));

    QImage loaded(path);
    QCOMPARE(loaded.size(), QSize(120, 80));
}

void TestImageExporter::testExportBMP() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QImage img(50, 50, QImage::Format_RGB32);
    img.fill(Qt::blue);

    QString path = dir.filePath("test.bmp");
    QVERIFY(ImageExporter::exportBMP(img, path));
    QVERIFY(QFile::exists(path));

    QImage loaded(path);
    QCOMPARE(loaded.size(), QSize(50, 50));
}

void TestImageExporter::testNullImageFails() {
    QTemporaryDir dir;
    QImage nullImg;
    QString path = dir.filePath("null.png");
    QVERIFY(!ImageExporter::exportPNG(nullImg, path));
}

void TestImageExporter::testDefaultFilename() {
    QString fnPng = ImageExporter::generateDefaultFilename("png");
    QVERIFY(fnPng.startsWith("LinuxShot_"));
    QVERIFY(fnPng.endsWith(".png"));

    QString fnJpg = ImageExporter::generateDefaultFilename("jpeg");
    QVERIFY(fnJpg.endsWith(".jpg"));
}

QTEST_MAIN(TestImageExporter)
#include "test_image_exporter.moc"
