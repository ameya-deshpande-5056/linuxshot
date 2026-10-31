#include "ExportManager.h"
#include "ImageExporter.h"
#include <QGuiApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>

ExportManager::ExportManager(QObject *parent)
    : QObject(parent)
{
}

bool ExportManager::saveToFile(const QImage &image, const QString &filePath, const QString &format, int quality) {
    if (image.isNull()) {
        emit saveFailed("Image is empty");
        return false;
    }

    bool success = ImageExporter::exportImage(image, filePath, format, quality);
    if (success) {
        emit imageSaved(filePath);
    } else {
        emit saveFailed(QString("Failed to write image to %1").arg(filePath));
    }
    return success;
}

QString ExportManager::saveWithDialog(const QImage &image, QWidget *parent, const QString &defaultDir, const QString &defaultFormat, int quality) {
    if (image.isNull()) {
        emit saveFailed("Image is empty");
        return QString();
    }

    QString dir = defaultDir;
    if (dir.isEmpty()) {
        dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }

    QString defaultFile = QDir(dir).filePath(ImageExporter::generateDefaultFilename(defaultFormat));

    QString selectedFilter;
    QString filter = "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp);;All Files (*)";

    QString filePath = QFileDialog::getSaveFileName(
        parent,
        "Save Screenshot As",
        defaultFile,
        filter,
        &selectedFilter
    );

    if (filePath.isEmpty()) {
        return QString(); // User cancelled
    }

    if (saveToFile(image, filePath, QString(), quality)) {
        return filePath;
    }
    return QString();
}

bool ExportManager::copyToClipboard(const QImage &image) {
    if (image.isNull()) return false;

    QClipboard *clipboard = QGuiApplication::clipboard();
    if (!clipboard) return false;

    clipboard->setImage(image, QClipboard::Clipboard);
    emit imageCopied();
    return true;
}
