#pragma once

#include <QObject>
#include <memory>
#include "UploadProvider.h"
#include "SelfHostedProvider.h"

class UploadManager : public QObject {
    Q_OBJECT

public:
    explicit UploadManager(QObject *parent = nullptr);
    ~UploadManager() override = default;

    void setEndpoint(const QString &url);
    QString endpoint() const;

    void upload(const QImage &image, const UploadOptions &opts);

signals:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void uploadCompleted(const UploadResult &result);

private:
    std::unique_ptr<SelfHostedProvider> m_provider;
};

