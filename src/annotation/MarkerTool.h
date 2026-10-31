#pragma once

#include "AnnotationTool.h"
#include <QPolygonF>

class MarkerCommand : public AnnotationCommand {
public:
    MarkerCommand(const QPolygonF &points, const QColor &color, int width);

    void paint(QPainter &painter) const override;
    QString description() const override { return "Highlighter Marker"; }
    std::unique_ptr<AnnotationCommand> clone() const override;

private:
    QPolygonF m_points;
    QColor m_color;
    int m_width;
};

class MarkerTool : public AnnotationTool {
public:
    MarkerTool();

    ToolType type() const override { return ToolType::Marker; }
    QString name() const override { return "Marker"; }

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
