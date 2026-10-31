#pragma once

#include "AnnotationTool.h"

class ArrowCommand : public AnnotationCommand {
public:
    ArrowCommand(const QPointF &start, const QPointF &end, const QColor &color, int width);

    void paint(QPainter &painter) const override;
    QString description() const override { return "Arrow"; }
    std::unique_ptr<AnnotationCommand> clone() const override;

private:
    QPointF m_start;
    QPointF m_end;
    QColor m_color;
    int m_width;
};

class ArrowTool : public AnnotationTool {
public:
    ToolType type() const override { return ToolType::Arrow; }
    QString name() const override { return "Arrow"; }

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
