#pragma once

#include "AnnotationTool.h"
#include <QPainterPath>
#include <QPolygonF>

class PenCommand : public AnnotationCommand {
public:
    PenCommand(const QPolygonF &points, const QColor &color, int width);

    void paint(QPainter &painter) const override;
    QString description() const override { return "Pen Stroke"; }
    std::unique_ptr<AnnotationCommand> clone() const override;

private:
    QPolygonF m_points;
    QColor m_color;
    int m_width;
};

class PenTool : public AnnotationTool {
public:
    ToolType type() const override { return ToolType::Pen; }
    QString name() const override { return "Pen"; }

    void mousePress(const QPointF &pos) override;
    void mouseMove(const QPointF &pos) override;
    void mouseRelease(const QPointF &pos) override;

    void paintPreview(QPainter &painter) const override;
    std::unique_ptr<AnnotationCommand> createCommand() override;
    bool isDrawing() const override { return m_drawing; }
    void reset() override;

private:
    QPolygonF m_currentPoints;
    bool m_drawing = false;
};

