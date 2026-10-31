#pragma once

#include <QImage>
#include <QRect>
#include <QList>
#include <QString>

class CaptureBackend {
public:
    virtual ~CaptureBackend() = default;

    virtual QString name() const = 0;
    virtual bool isAvailable() const = 0;

    virtual QImage captureScreen(int screenIndex = -1) = 0;
    virtual QImage captureFullDesktop() = 0;
    virtual QList<QRect> screenGeometries() = 0;
};

