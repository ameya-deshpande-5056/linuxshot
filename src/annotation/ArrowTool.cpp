#include "ArrowTool.h"
#include <cmath>
#include <QPolygonF>

ArrowCommand::ArrowCommand(const QPointF &start, const QPointF &end, const QColor &color, int width)
    : m_start(start), m_end(end), m_color(color), m_width(width)
{
}

void ArrowCommand::paint(QPainter &painter) const {
    qreal dx = m_end.x() - m_start.x();
    qreal dy = m_end.y() - m_start.y();
    qreal length = std::sqrt(dx * dx + dy * dy);
    if (length < 2.0) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(m_color, m_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(m_color);

    // Arrowhead dimensions proportional to line width
    qreal arrowSize = qMax(12.0, m_width * 3.5);
    arrowSize = qMin(arrowSize, length * 0.5);

    qreal angle = std::atan2(dy, dx);
    qreal arrowAngle = M_PI / 6.0; // 30 degrees

    // Base of arrowhead
    QPointF arrowP1 = m_end - QPointF(arrowSize * std::cos(angle - arrowAngle),
                                      arrowSize * std::sin(angle - arrowAngle));
    QPointF arrowP2 = m_end - QPointF(arrowSize * std::cos(angle + arrowAngle),
                                      arrowSize * std::sin(angle + arrowAngle));

    // Draw main line up to arrowhead base
    QPointF lineEnd = m_end - QPointF((arrowSize * 0.7) * std::cos(angle),
                                      (arrowSize * 0.7) * std::sin(angle));
    painter.drawLine(m_start, lineEnd);

    // Draw arrowhead polygon
    QPolygonF head;
    head << m_end << arrowP1 << arrowP2;
    painter.drawPolygon(head);

    painter.restore();
}

std::unique_ptr<AnnotationCommand> ArrowCommand::clone() const {
    return std::make_unique<ArrowCommand>(m_start, m_end, m_color, m_width);
}

void ArrowTool::mousePress(const QPointF &pos) {
    m_start = pos;
    m_current = pos;
    m_drawing = true;
}

void ArrowTool::mouseMove(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
}

void ArrowTool::mouseRelease(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
    m_drawing = false;
}

void ArrowTool::paintPreview(QPainter &painter) const {
    if (!m_drawing) return;
    ArrowCommand cmd(m_start, m_current, m_color, m_width);
    cmd.paint(painter);
}

std::unique_ptr<AnnotationCommand> ArrowTool::createCommand() {
    if (m_start == m_current) return nullptr;
    return std::make_unique<ArrowCommand>(m_start, m_current, m_color, m_width);
}

void ArrowTool::reset() {
    m_drawing = false;
}
