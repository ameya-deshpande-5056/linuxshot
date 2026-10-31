#include "IconHelper.h"
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QFile>
#include <QBuffer>
#include <QImageReader>

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

static QIcon loadSvgIcon(const QString &resourcePath, const QColor &color) {
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QIcon();
    }
    QByteArray data = file.readAll();
    file.close();

    // If it doesn't have custom gradients (like cancel.svg), tint black strokes and fills to the toolbar color
    if (!data.contains("linearGradient") && color.isValid()) {
        QByteArray hex = color.name().toUtf8();
        data.replace("#000000", hex);
        data.replace("var(--ci-primary-color, #000000)", hex);
    }

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer, "SVG");
    reader.setScaledSize(QSize(48, 48));
    QImage img = reader.read();

    if (img.isNull()) {
        QPixmap px;
        if (px.loadFromData(data, "SVG")) {
            return QIcon(px);
        }
        return QIcon();
    }

    return QIcon(QPixmap::fromImage(img));
}

QIcon IconHelper::actionIcon(Action action, const QColor &color) {
    QString path;
    switch (action) {
    case Action::Save:   path = ":/icons/linuxshot-ui-svgs/save.svg"; break;
    case Action::Copy:   path = ":/icons/linuxshot-ui-svgs/copy.svg"; break;
    case Action::Upload: path = ":/icons/linuxshot-ui-svgs/upload-svgrepo-com.svg"; break;
    case Action::Cancel: path = ":/icons/linuxshot-ui-svgs/cancel.svg"; break;
    case Action::Undo:   path = ":/icons/linuxshot-ui-svgs/undo-svgrepo-com.svg"; break;
    }

    if (!path.isEmpty()) {
        QIcon svgIcon = loadSvgIcon(path, color);
        if (!svgIcon.isNull()) {
            return svgIcon;
        }
    }

    // Fallback vector painter
    QPixmap pix = createBasePixmap(32);
    QPainter p(&pix);

    switch (action) {
    case Action::Save: {
        setupPainter(p, color, 1.8);
        p.drawRoundedRect(QRectF(4.5, 4.5, 23.0, 23.0), 2.0, 2.0);
        p.fillRect(QRectF(8.0, 4.5, 13.0, 9.0), color);
        p.fillRect(QRectF(15.0, 6.5, 3.5, 5.0), QColor("#2D2D2D"));
        p.drawRect(QRectF(7.5, 17.5, 17.0, 10.0));
        p.drawLine(QPointF(10.5, 21.0), QPointF(21.5, 21.0));
        p.drawLine(QPointF(10.5, 24.5), QPointF(18.5, 24.5));
        break;
    }
    case Action::Copy: {
        setupPainter(p, color, 1.8);
        p.drawRoundedRect(QRectF(11.5, 4.5, 15.0, 19.0), 1.5, 1.5);
        p.fillRect(QRectF(5.5, 8.5, 15.0, 19.0), QColor("#2D2D2D"));
        p.drawRoundedRect(QRectF(5.5, 8.5, 15.0, 19.0), 1.5, 1.5);
        p.drawLine(QPointF(9.0, 14.0), QPointF(17.0, 14.0));
        p.drawLine(QPointF(9.0, 18.0), QPointF(17.0, 18.0));
        p.drawLine(QPointF(9.0, 22.0), QPointF(14.0, 22.0));
        break;
    }
    case Action::Upload: {
        setupPainter(p, color, 2.0);
        QPainterPath cloud;
        cloud.moveTo(6.5, 21.5);
        cloud.arcTo(4.0, 14.0, 9.0, 9.0, 180, 150);
        cloud.arcTo(10.0, 8.0, 12.0, 12.0, 150, 180);
        cloud.arcTo(19.0, 13.0, 9.0, 9.0, 310, 150);
        p.drawPath(cloud);

        p.setBrush(color);
        p.setPen(Qt::NoPen);
        QPolygonF arrow;
        arrow << QPointF(16.0, 12.0)
              << QPointF(10.5, 18.0)
              << QPointF(13.5, 18.0)
              << QPointF(13.5, 25.5)
              << QPointF(18.5, 25.5)
              << QPointF(18.5, 18.0)
              << QPointF(21.5, 18.0);
        p.drawPolygon(arrow);
        break;
    }
    case Action::Cancel: {
        setupPainter(p, QColor("#F87171"), 2.4);
        p.drawLine(QPointF(7.5, 7.5), QPointF(24.5, 24.5));
        p.drawLine(QPointF(24.5, 7.5), QPointF(7.5, 24.5));
        break;
    }
    case Action::Undo: {
        setupPainter(p, color, 2.4);
        QPainterPath arc;
        arc.moveTo(23.5, 22.5);
        arc.arcTo(9.0, 9.0, 15.0, 15.0, 0, 180);
        p.drawPath(arc);

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
    QString path;
    switch (tool) {
    case ToolType::Pen:       path = ":/icons/linuxshot-ui-svgs/pen-clip-svgrepo-com.svg"; break;
    case ToolType::Line:      path = ":/icons/linuxshot-ui-svgs/line.svg"; break;
    case ToolType::Arrow:     path = ":/icons/linuxshot-ui-svgs/arrow-up-right.svg"; break;
    case ToolType::Rectangle: path = ":/icons/linuxshot-ui-svgs/rectangle-o.svg"; break;
    case ToolType::Circle:    path = ":/icons/linuxshot-ui-svgs/circle.svg"; break;
    case ToolType::Marker:    path = ":/icons/linuxshot-ui-svgs/marker-light-down-3.svg"; break;
    case ToolType::Text:      path = ":/icons/linuxshot-ui-svgs/text-svgrepo-com.svg"; break;
    case ToolType::Blur:      path = ":/icons/linuxshot-ui-svgs/blur.svg"; break;
    }

    if (!path.isEmpty()) {
        QIcon svgIcon = loadSvgIcon(path, color);
        if (!svgIcon.isNull()) {
            return svgIcon;
        }
    }

    // Fallback vector painter
    QPixmap pix = createBasePixmap(32);
    QPainter p(&pix);

    switch (tool) {
    case ToolType::Pen: {
        setupPainter(p, color, 1.8);
        p.drawLine(QPointF(7.0, 25.0), QPointF(22.0, 10.0));
        p.drawLine(QPointF(10.0, 25.0), QPointF(25.0, 10.0));
        p.drawLine(QPointF(22.0, 10.0), QPointF(25.0, 10.0));
        p.drawLine(QPointF(7.0, 25.0), QPointF(6.0, 26.0));
        p.drawLine(QPointF(10.0, 25.0), QPointF(6.0, 26.0));
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
        p.drawLine(QPointF(6.0, 26.0), QPointF(25.0, 7.0));
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
        QPainterPath body;
        body.moveTo(11.0, 25.0);
        body.lineTo(25.0, 11.0);
        body.lineTo(22.0, 8.0);
        body.lineTo(8.0, 22.0);
        body.closeSubpath();
        p.drawPath(body);
        p.fillRect(QRectF(6.0, 23.0, 5.0, 3.0), color);
        break;
    }
    case ToolType::Text: {
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setBrush(color);
        p.setPen(Qt::NoPen);
        QPainterPath path;
        path.addRoundedRect(QRectF(5.0, 7.0, 22.0, 4.5), 1.0, 1.0);
        path.addRoundedRect(QRectF(13.5, 10.5, 5.0, 14.5), 1.0, 1.0);
        path.addRoundedRect(QRectF(10.5, 22.5, 11.0, 3.0), 1.0, 1.0);
        p.drawPath(path);
        break;
    }
    case ToolType::Blur: {
        setupPainter(p, color, 1.0);
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
