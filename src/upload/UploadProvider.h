#pragma once

#include <QObject>
#include <QImage>
#include <QString>
#include <QUrl>

struct UploadResult {
    bool success = false;
    QUrl imageUrl;
    QUrl deleteUrl;
    QString errorMessage;
};

struct UploadOptions {
    QString format = "png";
    int jpegQuality = 90;
    QString customEndpoint;
};

class UploadProvider : public QObject {
    Q_OBJECT

public:
    explicit UploadProvider(QObject *parent = nullptr) : QObject(parent) {}
    ~UploadProvider() override;

    virtual QString name() const = 0;
    virtual bool isConfigured() const = 0;
    virtual void upload(const QImage &image, const UploadOptions &opts) = 0;

signals:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void uploadCompleted(const UploadResult &result);
};
