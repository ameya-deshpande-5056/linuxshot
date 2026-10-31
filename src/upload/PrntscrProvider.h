#pragma once

#include "UploadProvider.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

class PrntscrProvider : public UploadProvider {
    Q_OBJECT

public:
    explicit PrntscrProvider(QObject *parent = nullptr);
    ~PrntscrProvider() override = default;

    QString name() const override { return "Prntscr / Lightshot"; }
    bool isConfigured() const override { return true; }

    void upload(const QImage &image, const UploadOptions &opts) override;

    static QString generateHash(qint64 timestamp);
    static QString generateToken();

private slots:
    void onUploadFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager m_netManager;
};

