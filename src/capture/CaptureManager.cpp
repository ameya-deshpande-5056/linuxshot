#include "CaptureManager.h"
#include "X11CaptureBackend.h"
#include "PortalCaptureBackend.h"
#include "platform/Platform.h"
#include <QGuiApplication>
#include <QDebug>

CaptureManager::CaptureManager(QObject *parent)
    : QObject(parent)
{
    m_isWayland = Platform::isWayland();

    if (m_isWayland) {
        auto portal = std::make_unique<PortalCaptureBackend>(this);
        if (portal->isAvailable()) {
            m_backend = std::move(portal);
        }
    }

    if (!m_backend) {
        auto x11 = std::make_unique<X11CaptureBackend>();
        if (x11->isAvailable()) {
            m_backend = std::move(x11);
        }
    }

    if (!m_backend) {
        // Fallback default
        m_backend = std::make_unique<X11CaptureBackend>();
    }
}

QImage CaptureManager::captureDesktop() {
    if (!m_backend) {
        emit captureFailed("No capture backend available");
        return QImage();
    }

    QImage img = m_backend->captureFullDesktop();
    if (img.isNull()) {
        emit captureFailed("Failed to capture desktop screenshot");
    } else {
        emit captureCompleted(img);
    }
    return img;
}

QImage CaptureManager::captureMonitor(int monitorIndex) {
    if (!m_backend) {
        emit captureFailed("No capture backend available");
        return QImage();
    }

    QImage img = m_backend->captureScreen(monitorIndex);
    if (img.isNull()) {
        emit captureFailed(QString("Failed to capture monitor %1").arg(monitorIndex));
    } else {
        emit captureCompleted(img);
    }
    return img;
}

QList<QRect> CaptureManager::screenGeometries() const {
    if (m_backend) {
        return m_backend->screenGeometries();
    }
    return {};
}

QString CaptureManager::activeBackendName() const {
    return m_backend ? m_backend->name() : "None";
}

bool CaptureManager::isWayland() const {
    return m_isWayland;
}

