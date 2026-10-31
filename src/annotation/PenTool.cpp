#include "PenTool.h"

PenCommand::PenCommand(const QPolygonF &points, const QColor &color, int width)
    : m_points(points), m_color(color), m_width(width)
{
}

void PenCommand::paint(QPainter &painter) const {
    if (m_points.size() < 2) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(m_color, m_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    QPainterPath path;
    path.moveTo(m_points.first());
    for (int i = 1; i < m_points.size(); ++i) {
        path.lineTo(m_points.at(i));
    }
    painter.drawPath(path);
    painter.restore();
}

std::unique_ptr<AnnotationCommand> PenCommand::clone() const {
    return std::make_unique<PenCommand>(m_points, m_color, m_width);
}

void PenTool::mousePress(const QPointF &pos) {
    m_currentPoints.clear();
    m_currentPoints.append(pos);
    m_drawing = true;
}

void PenTool::mouseMove(const QPointF &pos) {
    if (!m_drawing) return;
    m_currentPoints.append(pos);
}

void PenTool::mouseRelease(const QPointF &pos) {
    if (!m_drawing) return;
    m_currentPoints.append(pos);
    m_drawing = false;
}

void PenTool::paintPreview(QPainter &painter) const {
    if (m_currentPoints.size() < 2) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(m_color, m_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    QPainterPath path;
    path.moveTo(m_currentPoints.first());
    for (int i = 1; i < m_currentPoints.size(); ++i) {
        path.lineTo(m_currentPoints.at(i));
    }
    painter.drawPath(path);
    painter.restore();
}

std::unique_ptr<AnnotationCommand> PenTool::createCommand() {
    if (m_currentPoints.size() < 2) {
        m_currentPoints.clear();
        return nullptr;
    }
    auto cmd = std::make_unique<PenCommand>(m_currentPoints, m_color, m_width);
    m_currentPoints.clear();
    return cmd;
}

void PenTool::reset() {
    m_currentPoints.clear();
    m_drawing = false;
}
