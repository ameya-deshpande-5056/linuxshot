#pragma once

#include "CaptureBackend.h"
#include <QObject>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>

class PortalCaptureBackend : public QObject, public CaptureBackend {
    Q_OBJECT

public:
    explicit PortalCaptureBackend(QObject *parent = nullptr);
    ~PortalCaptureBackend() override = default;

    QString name() const override { return "XDG Desktop Portal (Wayland)"; }
    bool isAvailable() const override;

    QImage captureScreen(int screenIndex = -1) override;
    QImage captureFullDesktop() override;
    QList<QRect> screenGeometries() override;

private:
    bool m_portalAvailable = false;
};

