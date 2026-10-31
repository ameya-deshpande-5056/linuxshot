#include "CircleTool.h"

CircleCommand::CircleCommand(const QRectF &rect, const QColor &color, int width)
    : m_rect(rect.normalized()), m_color(color), m_width(width)
{
}

void CircleCommand::paint(QPainter &painter) const {
    if (m_rect.isEmpty()) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(m_color, m_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(m_rect);
    painter.restore();
}

std::unique_ptr<AnnotationCommand> CircleCommand::clone() const {
    return std::make_unique<CircleCommand>(m_rect, m_color, m_width);
}

void CircleTool::mousePress(const QPointF &pos) {
    m_start = pos;
    m_current = pos;
    m_drawing = true;
}

void CircleTool::mouseMove(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
}

void CircleTool::mouseRelease(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
    m_drawing = false;
}

void CircleTool::paintPreview(QPainter &painter) const {
    if (!m_drawing) return;
    QRectF r(m_start, m_current);
    CircleCommand cmd(r, m_color, m_width);
    cmd.paint(painter);
}

std::unique_ptr<AnnotationCommand> CircleTool::createCommand() {
    QRectF r(m_start, m_current);
    if (r.normalized().width() < 2 || r.normalized().height() < 2) {
        return nullptr;
    }
    return std::make_unique<CircleCommand>(r, m_color, m_width);
}

void CircleTool::reset() {
    m_drawing = false;
}
