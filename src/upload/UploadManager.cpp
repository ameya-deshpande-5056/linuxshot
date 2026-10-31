#include "UploadManager.h"

UploadManager::UploadManager(QObject *parent)
    : QObject(parent)
{
    m_selfHostedProvider = std::make_unique<SelfHostedProvider>(this);
    m_prntscrProvider = std::make_unique<PrntscrProvider>(this);

    connect(m_selfHostedProvider.get(), &UploadProvider::uploadProgress, this, &UploadManager::uploadProgress);
    connect(m_selfHostedProvider.get(), &UploadProvider::uploadCompleted, this, &UploadManager::uploadCompleted);

    connect(m_prntscrProvider.get(), &UploadProvider::uploadProgress, this, &UploadManager::uploadProgress);
    connect(m_prntscrProvider.get(), &UploadProvider::uploadCompleted, this, &UploadManager::uploadCompleted);
}

void UploadManager::setEndpoint(const QString &url) {
    if (m_selfHostedProvider) {
        m_selfHostedProvider->setEndpoint(url);
    }
}

QString UploadManager::endpoint() const {
    return m_selfHostedProvider ? m_selfHostedProvider->endpoint() : QString();
}

void UploadManager::setPrntscrEnabled(bool enabled) {
    m_usePrntscr = enabled;
}

bool UploadManager::prntscrEnabled() const {
    return m_usePrntscr;
}

void UploadManager::upload(const QImage &image, const UploadOptions &opts) {
    if (m_usePrntscr && m_prntscrProvider) {
        m_prntscrProvider->upload(image, opts);
    } else if (m_selfHostedProvider) {
        m_selfHostedProvider->upload(image, opts);
    }
}
