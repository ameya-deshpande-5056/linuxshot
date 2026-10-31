#pragma once

#include <QPainter>
#include <QImage>
#include <QString>
#include <memory>

class AnnotationCommand {
public:
    virtual ~AnnotationCommand() = default;

    // Paint non-destructively onto a painter (for display)
    virtual void paint(QPainter &painter) const = 0;

    // Direct pixel modification if required (e.g. for blur/pixelation)
    virtual void applyToImage(QImage &image) const {
        QPainter p(&image);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        paint(p);
    }

    virtual QString description() const = 0;
    virtual std::unique_ptr<AnnotationCommand> clone() const = 0;
};
