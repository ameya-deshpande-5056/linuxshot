#include "ImageExporter.h"
#include <QFileInfo>
#include <QDateTime>
#include <QImageWriter>

bool ImageExporter::exportImage(const QImage &image, const QString &filePath, const QString &format, int quality) {
    if (image.isNull() || filePath.isEmpty()) {
        return false;
    }

    QFileInfo info(filePath);
    QString fmt = format;
    if (fmt.isEmpty()) {
        fmt = info.suffix().toLower();
    }
    if (fmt.isEmpty()) {
        fmt = "png";
    }

    QImageWriter writer(filePath, fmt.toUtf8());
    if (quality >= 0) {
        writer.setQuality(quality);
    }
    return writer.write(image);
}

bool ImageExporter::exportPNG(const QImage &image, const QString &filePath) {
    return exportImage(image, filePath, "png", 100);
}

bool ImageExporter::exportJPEG(const QImage &image, const QString &filePath, int quality) {
    return exportImage(image, filePath, "jpg", quality);
}

bool ImageExporter::exportBMP(const QImage &image, const QString &filePath) {
    return exportImage(image, filePath, "bmp", -1);
}

QStringList ImageExporter::supportedFormats() {
    return {"png", "jpg", "jpeg", "bmp"};
}

QString ImageExporter::defaultExtension(const QString &format) {
    QString f = format.toLower();
    if (f == "jpeg" || f == "jpg") return "jpg";
    if (f == "bmp") return "bmp";
    return "png";
}

QString ImageExporter::generateDefaultFilename(const QString &format) {
    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    return QString("LinuxShot_%1.%2").arg(timeStr, defaultExtension(format));
}

