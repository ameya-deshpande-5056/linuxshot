#pragma once

#include "AnnotationTool.h"

class TextCommand : public AnnotationCommand {
public:
    TextCommand(const QString &text, const QPointF &pos, const QFont &font, const QColor &color);

    void paint(QPainter &painter) const override;
    QString description() const override { return "Text"; }
    std::unique_ptr<AnnotationCommand> clone() const override;

private:
    QString m_text;
    QPointF m_pos;
    QFont m_font;
    QColor m_color;
};

class TextTool : public AnnotationTool {
public:
    ToolType type() const override { return ToolType::Text; }
    QString name() const override { return "Text"; }

    void mousePress(const QPointF &pos) override;
    void mouseMove(const QPointF &pos) override;
    void mouseRelease(const QPointF &pos) override;

    void paintPreview(QPainter &painter) const override;
    std::unique_ptr<AnnotationCommand> createCommand() override;
    bool isDrawing() const override { return m_active; }
    void reset() override;

    void setText(const QString &text) { m_text = text; }
    QPointF clickPos() const { return m_clickPos; }

private:
    QPointF m_clickPos;
    QString m_text;
    bool m_active = false;
};

