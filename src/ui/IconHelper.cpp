#include "IconHelper.h"
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>

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

        // Cloud contour over the top
        QPainterPath cloud;
        cloud.moveTo(6.5, 21.5);
        cloud.arcTo(4.0, 14.0, 9.0, 9.0, 180, 150);
        cloud.arcTo(10.0, 8.0, 12.0, 12.0, 150, 180);
        cloud.arcTo(19.0, 13.0, 9.0, 9.0, 310, 150);
        p.drawPath(cloud);

        // Bold solid upward arrow pointing into the cloud
        p.setBrush(color);
        p.setPen(Qt::NoPen);
        QPolygonF arrow;
        arrow << QPointF(16.0, 12.0)   // top tip
              << QPointF(10.5, 18.0)   // left wing
              << QPointF(13.5, 18.0)   // inner left notch
              << QPointF(13.5, 25.5)   // bottom left stem
              << QPointF(18.5, 25.5)   // bottom right stem
              << QPointF(18.5, 18.0)   // inner right notch
              << QPointF(21.5, 18.0);  // right wing
        p.drawPolygon(arrow);
        break;
    }
    case Action::Cancel: {
        setupPainter(p, QColor("#F87171"), 2.4); // High-visibility red cross
        p.drawLine(QPointF(7.5, 7.5), QPointF(24.5, 24.5));
        p.drawLine(QPointF(24.5, 7.5), QPointF(7.5, 24.5));
        break;
    }
    case Action::Undo: {
        setupPainter(p, color, 2.4);

        // Smooth curved arc
        QPainterPath arc;
        arc.moveTo(23.5, 22.5);
        arc.arcTo(9.0, 9.0, 15.0, 15.0, 0, 180);
        p.drawPath(arc);

        // Solid filled arrowhead pointing left
        p.setBrush(color);
        p.setPen(Qt::NoPen);
        QPolygonF head;
        head << QPointF(5.0, 16.5)
             << QPointF(12.5, 10.5)
             << QPointF(12.5, 22.5);
        p.drawPolygon(head);
        break;
    }
    }

    return QIcon(pix);
}

QIcon IconHelper::toolIcon(ToolType tool, const QColor &color) {
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
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setBrush(color);
        p.setPen(Qt::NoPen);

        // Clean, bold letter 'T' with serifs
        QPainterPath path;
        // Top crossbar
        path.addRoundedRect(QRectF(5.0, 7.0, 22.0, 4.5), 1.0, 1.0);
        // Vertical stem
        path.addRoundedRect(QRectF(13.5, 10.5, 5.0, 14.5), 1.0, 1.0);
        // Base serifs
        path.addRoundedRect(QRectF(10.5, 22.5, 11.0, 3.0), 1.0, 1.0);
        p.drawPath(path);
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
