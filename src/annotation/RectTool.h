#pragma once

#include "AnnotationTool.h"
#include <QRectF>

class RectCommand : public AnnotationCommand {
public:
    RectCommand(const QRectF &rect, const QColor &color, int width);

    void paint(QPainter &painter) const override;
    QString description() const override { return "Rectangle"; }
    std::unique_ptr<AnnotationCommand> clone() const override;

private:
    QRectF m_rect;
    QColor m_color;
    int m_width;
};

class RectTool : public AnnotationTool {
public:
    ToolType type() const override { return ToolType::Rectangle; }
    QString name() const override { return "Rectangle"; }

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
