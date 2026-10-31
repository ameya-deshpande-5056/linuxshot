#pragma once

#include <QObject>
#include <QImage>
#include <QRect>
#include <memory>
#include "CaptureBackend.h"

class CaptureManager : public QObject {
    Q_OBJECT

public:
    explicit CaptureManager(QObject *parent = nullptr);
    ~CaptureManager() override = default;

    QImage captureDesktop();
    QImage captureMonitor(int monitorIndex);
    QList<QRect> screenGeometries() const;

    QString activeBackendName() const;
    bool isWayland() const;

signals:
    void captureCompleted(const QImage &screenshot);
    void captureFailed(const QString &errorMessage);

private:
    std::unique_ptr<CaptureBackend> m_backend;
    bool m_isWayland = false;
};

