#include "LineTool.h"

LineCommand::LineCommand(const QPointF &start, const QPointF &end, const QColor &color, int width)
    : m_start(start), m_end(end), m_color(color), m_width(width)
{
}

void LineCommand::paint(QPainter &painter) const {
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(m_color, m_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.drawLine(m_start, m_end);
    painter.restore();
}

std::unique_ptr<AnnotationCommand> LineCommand::clone() const {
    return std::make_unique<LineCommand>(m_start, m_end, m_color, m_width);
}

void LineTool::mousePress(const QPointF &pos) {
    m_start = pos;
    m_current = pos;
    m_drawing = true;
}

void LineTool::mouseMove(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
}

void LineTool::mouseRelease(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
    m_drawing = false;
}

void LineTool::paintPreview(QPainter &painter) const {
    if (!m_drawing) return;
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(m_color, m_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.drawLine(m_start, m_current);
    painter.restore();
}

std::unique_ptr<AnnotationCommand> LineTool::createCommand() {
    if (m_start == m_current) {
        return nullptr;
    }
    return std::make_unique<LineCommand>(m_start, m_current, m_color, m_width);
}

void LineTool::reset() {
    m_drawing = false;
}
