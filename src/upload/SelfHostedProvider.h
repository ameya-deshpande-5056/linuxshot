#pragma once

#include "UploadProvider.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

class SelfHostedProvider : public UploadProvider {
    Q_OBJECT

public:
    explicit SelfHostedProvider(QObject *parent = nullptr);
    ~SelfHostedProvider() override = default;

    QString name() const override { return "HTTP Multipart Provider"; }
    bool isConfigured() const override;

    void setEndpoint(const QString &url) { m_endpoint = url; }
    QString endpoint() const { return m_endpoint; }

    void upload(const QImage &image, const UploadOptions &opts) override;

private slots:
    void onUploadFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager m_netManager;
    QString m_endpoint;
};
