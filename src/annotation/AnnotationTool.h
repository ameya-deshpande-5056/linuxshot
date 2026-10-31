#pragma once

#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QFont>
#include <memory>
#include "AnnotationCommand.h"

enum class ToolType {
    Pen,
    Line,
    Arrow,
    Rectangle,
    Circle,
    Marker,
    Text,
    Blur
};

class AnnotationTool {
public:
    virtual ~AnnotationTool() = default;

    virtual ToolType type() const = 0;
    virtual QString name() const = 0;

    virtual void mousePress(const QPointF &pos) = 0;
    virtual void mouseMove(const QPointF &pos) = 0;
    virtual void mouseRelease(const QPointF &pos) = 0;

    virtual void paintPreview(QPainter &painter) const = 0;
    virtual std::unique_ptr<AnnotationCommand> createCommand() = 0;
    virtual bool isDrawing() const = 0;
    virtual void reset() = 0;

    void setColor(const QColor &color) { m_color = color; }
    QColor color() const { return m_color; }

    void setWidth(int width) { m_width = qMax(1, width); }
    int width() const { return m_width; }

    void setFont(const QFont &font) { m_font = font; }
    QFont font() const { return m_font; }

protected:
    QColor m_color = Qt::red;
    int m_width = 3;
    QFont m_font = QFont("Sans Serif", 14, QFont::Bold);
};
