#pragma once

#include <QImage>
#include <QString>
#include <QStringList>

class ImageExporter {
public:
    static bool exportImage(const QImage &image, const QString &filePath, const QString &format = QString(), int quality = -1);

    static bool exportPNG(const QImage &image, const QString &filePath);
    static bool exportJPEG(const QImage &image, const QString &filePath, int quality = 90);
    static bool exportBMP(const QImage &image, const QString &filePath);

    static QStringList supportedFormats();
    static QString defaultExtension(const QString &format);
    static QString generateDefaultFilename(const QString &format = "png");
};
