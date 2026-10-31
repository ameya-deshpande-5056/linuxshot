#include "IconHelper.h"
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QFile>

static QPixmap createBasePixmap(int size = 32) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    return pix;
}

static void setupPainter(QPainter &p, const QColor &color, double penWidth = 2.0) {
    p.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(color, penWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
}

QIcon IconHelper::actionIcon(Action action, const QColor &color) {
    QString resPath;
    switch (action) {
    case Action::Save:   resPath = ":/icons/lightshot/save.png"; break;
    case Action::Copy:   resPath = ":/icons/lightshot/copy.png"; break;
    case Action::Upload: resPath = ":/icons/lightshot/upload.png"; break;
    case Action::Cancel: resPath = ":/icons/lightshot/cancel.png"; break;
    case Action::Undo:   resPath = ":/icons/lightshot/undo.png"; break;
    }

    if (!resPath.isEmpty() && QFile::exists(resPath)) {
        return QIcon(resPath);
    }

    QPixmap pix = createBasePixmap(32);
    QPainter p(&pix);

    switch (action) {
    case Action::Save: {
        setupPainter(p, color, 1.8);
        // Outer floppy disk body
        p.drawRoundedRect(QRectF(4.5, 4.5, 23.0, 23.0), 2.0, 2.0);

        // Top shutter (sliding metal sleeve)
        p.fillRect(QRectF(8.0, 4.5, 13.0, 9.0), color);
        // Shutter cutout notch
        p.fillRect(QRectF(15.0, 6.5, 3.5, 5.0), QColor("#2D2D2D"));

        // Bottom paper write-label
        p.drawRect(QRectF(7.5, 17.5, 17.0, 10.0));
        // Label text lines
        p.drawLine(QPointF(10.5, 21.0), QPointF(21.5, 21.0));
        p.drawLine(QPointF(10.5, 24.5), QPointF(18.5, 24.5));
        break;
    }
    case Action::Copy: {
        setupPainter(p, color, 1.8);
        // Back page (top-right)
        p.drawRoundedRect(QRectF(11.5, 4.5, 15.0, 19.0), 1.5, 1.5);

        // Front page (bottom-left)
        p.fillRect(QRectF(5.5, 8.5, 15.0, 19.0), QColor("#2D2D2D"));
        p.drawRoundedRect(QRectF(5.5, 8.5, 15.0, 19.0), 1.5, 1.5);

        // Document text lines on front page
        p.drawLine(QPointF(9.0, 14.0), QPointF(17.0, 14.0));
        p.drawLine(QPointF(9.0, 18.0), QPointF(17.0, 18.0));
        p.drawLine(QPointF(9.0, 22.0), QPointF(14.0, 22.0));
        break;
    }
    case Action::Upload: {
        setupPainter(p, color, 2.0);
        // Cloud outline
        QPainterPath cloud;
        cloud.moveTo(6.0, 22.0);
        cloud.arcTo(3.0, 15.0, 9.0, 9.0, 200, 150);
        cloud.arcTo(9.0, 7.0, 14.0, 14.0, 150, 180);
        cloud.arcTo(19.0, 13.0, 10.0, 10.0, 300, 150);
        p.drawPath(cloud);

        // Upload arrow in center
        p.drawLine(QPointF(16.0, 12.0), QPointF(16.0, 26.0));
        p.drawLine(QPointF(11.0, 17.0), QPointF(16.0, 12.0));
        p.drawLine(QPointF(21.0, 17.0), QPointF(16.0, 12.0));
        break;
    }
    case Action::Cancel: {
        setupPainter(p, QColor("#F87171"), 2.4); // Red cross
        p.drawLine(QPointF(7.5, 7.5), QPointF(24.5, 24.5));
        p.drawLine(QPointF(24.5, 7.5), QPointF(7.5, 24.5));
        break;
    }
    case Action::Undo: {
        setupPainter(p, color, 2.0);
        // Curved arc
        QPainterPath arc;
        arc.moveTo(25.0, 24.0);
        arc.arcTo(7.0, 9.0, 18.0, 18.0, 0, 190);
        p.drawPath(arc);

        // Arrowhead
        p.drawLine(QPointF(7.0, 18.0), QPointF(4.0, 11.0));
        p.drawLine(QPointF(13.0, 11.0), QPointF(4.0, 11.0));
        break;
    }
    }

    return QIcon(pix);
}

QIcon IconHelper::toolIcon(ToolType tool, const QColor &color) {
    QString resPath;
    switch (tool) {
    case ToolType::Pen:       resPath = ":/icons/lightshot/pen.png"; break;
    case ToolType::Line:      resPath = ":/icons/lightshot/line.png"; break;
    case ToolType::Arrow:     resPath = ":/icons/lightshot/arrow.png"; break;
    case ToolType::Rectangle: resPath = ":/icons/lightshot/rect.png"; break;
    case ToolType::Circle:    resPath = ":/icons/lightshot/circle.png"; break;
    case ToolType::Marker:    resPath = ":/icons/lightshot/marker.png"; break;
    case ToolType::Text:      resPath = ":/icons/lightshot/text.png"; break;
    case ToolType::Blur:      resPath = ":/icons/lightshot/blur.png"; break;
    }

    if (!resPath.isEmpty() && QFile::exists(resPath)) {
        return QIcon(resPath);
    }

    QPixmap pix = createBasePixmap(32);
    QPainter p(&pix);

    switch (tool) {
    case ToolType::Pen: {
        setupPainter(p, color, 1.8);
        // Angled pencil body
        p.drawLine(QPointF(7.0, 25.0), QPointF(22.0, 10.0));
        p.drawLine(QPointF(10.0, 25.0), QPointF(25.0, 10.0));
        p.drawLine(QPointF(22.0, 10.0), QPointF(25.0, 10.0));
        // Nib point
        p.drawLine(QPointF(7.0, 25.0), QPointF(6.0, 26.0));
        p.drawLine(QPointF(10.0, 25.0), QPointF(6.0, 26.0));
        // Eraser top
        p.drawLine(QPointF(22.0, 10.0), QPointF(24.0, 8.0));
        p.drawLine(QPointF(25.0, 10.0), QPointF(27.0, 8.0));
        p.drawLine(QPointF(24.0, 8.0), QPointF(27.0, 8.0));
        break;
    }
    case ToolType::Line: {
        setupPainter(p, color, 2.2);
        p.drawLine(QPointF(6.0, 26.0), QPointF(26.0, 6.0));
        break;
    }
    case ToolType::Arrow: {
        setupPainter(p, color, 2.2);
        // Shaft
        p.drawLine(QPointF(6.0, 26.0), QPointF(25.0, 7.0));
        // Arrowhead
        p.drawLine(QPointF(15.0, 7.0), QPointF(25.0, 7.0));
        p.drawLine(QPointF(25.0, 17.0), QPointF(25.0, 7.0));
        break;
    }
    case ToolType::Rectangle: {
        setupPainter(p, color, 2.0);
        p.drawRoundedRect(QRectF(5.5, 7.5, 21.0, 17.0), 1.5, 1.5);
        break;
    }
    case ToolType::Circle: {
        setupPainter(p, color, 2.0);
        p.drawEllipse(QRectF(5.5, 5.5, 21.0, 21.0));
        break;
    }
    case ToolType::Marker: {
        setupPainter(p, color, 2.0);
        // Slanted highlighter body
        QPainterPath body;
        body.moveTo(11.0, 25.0);
        body.lineTo(25.0, 11.0);
        body.lineTo(22.0, 8.0);
        body.lineTo(8.0, 22.0);
        body.closeSubpath();
        p.drawPath(body);

        // Chisel tip
        p.fillRect(QRectF(6.0, 23.0, 5.0, 3.0), color);
        break;
    }
    case ToolType::Text: {
        setupPainter(p, color, 2.4);
        // Bold letter 'T'
        p.drawLine(QPointF(6.0, 8.0), QPointF(26.0, 8.0));
        p.drawLine(QPointF(16.0, 8.0), QPointF(16.0, 25.0));
        // Bottom base serifs
        p.drawLine(QPointF(12.0, 25.0), QPointF(20.0, 25.0));
        break;
    }
    case ToolType::Blur: {
        setupPainter(p, color, 1.0);
        // 3x3 pixel grid pattern
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                QRectF cell(6.0 + c * 7.0, 6.0 + r * 7.0, 6.0, 6.0);
                if ((r + c) % 2 == 0) {
                    p.fillRect(cell, color);
                } else {
                    p.drawRect(cell);
                }
            }
        }
        break;
    }
    }

    return QIcon(pix);
}
