#pragma once

#include <QObject>
#include <QImage>
#include <QWidget>

class ExportManager : public QObject {
    Q_OBJECT

public:
    explicit ExportManager(QObject *parent = nullptr);
    ~ExportManager() override = default;

    bool saveToFile(const QImage &image, const QString &filePath, const QString &format = QString(), int quality = -1);
    QString saveWithDialog(const QImage &image, QWidget *parent = nullptr, const QString &defaultDir = QString(), const QString &defaultFormat = "png", int quality = 90);
    bool copyToClipboard(const QImage &image);

signals:
    void imageSaved(const QString &filePath);
    void imageCopied();
    void saveFailed(const QString &errorMessage);
};
