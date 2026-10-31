#pragma once

#include "AnnotationTool.h"

class TextCommand : public AnnotationCommand {
public:
    TextCommand(const QString &text, const QPointF &pos, const QFont &font, const QColor &color, int boxWidth = 0);

    void paint(QPainter &painter) const override;
    QString description() const override { return "Text"; }
    std::unique_ptr<AnnotationCommand> clone() const override;

    QString text() const { return m_text; }
    QPointF pos() const { return m_pos; }
    QFont font() const { return m_font; }
    QColor color() const { return m_color; }
    int boxWidth() const { return m_boxWidth; }

    static QString wrapText(const QString &text, const QFont &font, qreal wrapWidth);

private:
    QString m_text;
    QPointF m_pos;
    QFont m_font;
    QColor m_color;
    int m_boxWidth = 0;
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

