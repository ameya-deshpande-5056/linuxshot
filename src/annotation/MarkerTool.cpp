#include "MarkerTool.h"
#include <QPainterPath>

MarkerCommand::MarkerCommand(const QPolygonF &points, const QColor &color, int width)
    : m_points(points), m_color(color), m_width(width)
{
    // Ensure semi-transparent alpha for highlighter effect
    m_color.setAlpha(90);
}

void MarkerCommand::paint(QPainter &painter) const {
    if (m_points.size() < 2) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

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

std::unique_ptr<AnnotationCommand> MarkerCommand::clone() const {
    return std::make_unique<MarkerCommand>(m_points, m_color, m_width);
}

MarkerTool::MarkerTool() {
    m_color = QColor(255, 235, 59); // Highlighter yellow default
    m_width = 24;
}

void MarkerTool::mousePress(const QPointF &pos) {
    m_currentPoints.clear();
    m_currentPoints.append(pos);
    m_drawing = true;
}

void MarkerTool::mouseMove(const QPointF &pos) {
    if (!m_drawing) return;
    m_currentPoints.append(pos);
}

void MarkerTool::mouseRelease(const QPointF &pos) {
    if (!m_drawing) return;
    m_currentPoints.append(pos);
    m_drawing = false;
}

void MarkerTool::paintPreview(QPainter &painter) const {
    if (m_currentPoints.size() < 2) return;
    MarkerCommand cmd(m_currentPoints, m_color, m_width);
    cmd.paint(painter);
}

std::unique_ptr<AnnotationCommand> MarkerTool::createCommand() {
    if (m_currentPoints.size() < 2) {
        m_currentPoints.clear();
        return nullptr;
    }
    auto cmd = std::make_unique<MarkerCommand>(m_currentPoints, m_color, m_width);
    m_currentPoints.clear();
    return cmd;
}

void MarkerTool::reset() {
    m_currentPoints.clear();
    m_drawing = false;
}
