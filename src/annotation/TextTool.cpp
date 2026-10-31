#include "TextTool.h"
#include <QPainterPath>

TextCommand::TextCommand(const QString &text, const QPointF &pos, const QFont &font, const QColor &color)
    : m_text(text), m_pos(pos), m_font(font), m_color(color)
{
}

void TextCommand::paint(QPainter &painter) const {
    if (m_text.isEmpty()) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setFont(m_font);
    painter.setPen(m_color);

    // Draw text with a subtle dark outline for readability on all backgrounds
    QPainterPath path;
    path.addText(m_pos, m_font, m_text);

    QPen outlinePen(QColor(0, 0, 0, 160), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.strokePath(path, outlinePen);
    painter.fillPath(path, m_color);

    painter.restore();
}

std::unique_ptr<AnnotationCommand> TextCommand::clone() const {
    return std::make_unique<TextCommand>(m_text, m_pos, m_font, m_color);
}

void TextTool::mousePress(const QPointF &pos) {
    m_clickPos = pos;
    m_active = true;
}

void TextTool::mouseMove(const QPointF &pos) {
    Q_UNUSED(pos);
}

void TextTool::mouseRelease(const QPointF &pos) {
    Q_UNUSED(pos);
}

void TextTool::paintPreview(QPainter &painter) const {
    if (!m_active || m_text.isEmpty()) return;
    TextCommand cmd(m_text, m_clickPos, m_font, m_color);
    cmd.paint(painter);
}

std::unique_ptr<AnnotationCommand> TextTool::createCommand() {
    if (m_text.trimmed().isEmpty()) {
        m_active = false;
        return nullptr;
    }
    auto cmd = std::make_unique<TextCommand>(m_text, m_clickPos, m_font, m_color);
    m_text.clear();
    m_active = false;
    return cmd;
}

void TextTool::reset() {
    m_text.clear();
    m_active = false;
}
