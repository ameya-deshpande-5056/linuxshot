#include "UploadManager.h"

UploadManager::UploadManager(QObject *parent)
    : QObject(parent)
{
    m_provider = std::make_unique<SelfHostedProvider>(this);

    connect(m_provider.get(), &UploadProvider::uploadProgress, this, &UploadManager::uploadProgress);
    connect(m_provider.get(), &UploadProvider::uploadCompleted, this, &UploadManager::uploadCompleted);
}

void UploadManager::setEndpoint(const QString &url) {
    if (m_provider) {
        m_provider->setEndpoint(url);
    }
}

QString UploadManager::endpoint() const {
    return m_provider ? m_provider->endpoint() : QString();
}

void UploadManager::upload(const QImage &image, const UploadOptions &opts) {
    if (m_provider) {
        m_provider->upload(image, opts);
    }
}
