#pragma once

#include "AnnotationTool.h"
#include <QRectF>
#include <QPixmap>

class BlurCommand : public AnnotationCommand {
public:
    BlurCommand(const QRectF &rect, int blockSize = 12);

    void paint(QPainter &painter) const override;
    void applyToImage(QImage &image) const override;
    QString description() const override { return "Blur / Pixelate"; }
    std::unique_ptr<AnnotationCommand> clone() const override;

    void cachePixelation(const QImage &sourceImage);

private:
    QRectF m_rect;
    int m_blockSize;
    mutable QPixmap m_cachedMosaic;
};

class BlurTool : public AnnotationTool {
public:
    ToolType type() const override { return ToolType::Blur; }
    QString name() const override { return "Blur"; }

    void mousePress(const QPointF &pos) override;
    void mouseMove(const QPointF &pos) override;
    void mouseRelease(const QPointF &pos) override;

    void paintPreview(QPainter &painter) const override;
    std::unique_ptr<AnnotationCommand> createCommand() override;
    bool isDrawing() const override { return m_drawing; }
    void reset() override;

private:
    QPointF m_start;
    QPointF m_current;
    bool m_drawing = false;
};

