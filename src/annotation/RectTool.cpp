#include "RectTool.h"

RectCommand::RectCommand(const QRectF &rect, const QColor &color, int width)
    : m_rect(rect.normalized()), m_color(color), m_width(width)
{
}

void RectCommand::paint(QPainter &painter) const {
    if (m_rect.isEmpty()) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(m_color, m_width, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(m_rect);
    painter.restore();
}

std::unique_ptr<AnnotationCommand> RectCommand::clone() const {
    return std::make_unique<RectCommand>(m_rect, m_color, m_width);
}

void RectTool::mousePress(const QPointF &pos) {
    m_start = pos;
    m_current = pos;
    m_drawing = true;
}

void RectTool::mouseMove(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
}

void RectTool::mouseRelease(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
    m_drawing = false;
}

void RectTool::paintPreview(QPainter &painter) const {
    if (!m_drawing) return;
    QRectF r(m_start, m_current);
    RectCommand cmd(r, m_color, m_width);
    cmd.paint(painter);
}

std::unique_ptr<AnnotationCommand> RectTool::createCommand() {
    QRectF r(m_start, m_current);
    if (r.normalized().width() < 2 || r.normalized().height() < 2) {
        return nullptr;
    }
    return std::make_unique<RectCommand>(r, m_color, m_width);
}

void RectTool::reset() {
    m_drawing = false;
}
