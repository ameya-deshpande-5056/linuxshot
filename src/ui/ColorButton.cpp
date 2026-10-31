#include "ColorButton.h"
#include <QColorDialog>
#include <QPainter>

ColorButton::ColorButton(QWidget *parent)
    : ColorButton(Qt::red, parent)
{
}

ColorButton::ColorButton(const QColor &color, QWidget *parent)
    : QPushButton(parent), m_color(color)
{
    setFixedSize(28, 28);
    setCursor(Qt::PointingHandCursor);
    setToolTip("Choose Annotation Color");
    connect(this, &QPushButton::clicked, this, &ColorButton::chooseColor);
}

void ColorButton::setColor(const QColor &color) {
    if (m_color != color) {
        m_color = color;
        update();
        emit colorChanged(m_color);
    }
}

void ColorButton::chooseColor() {
    QColor chosen = QColorDialog::getColor(m_color, this, "Select Color");
    if (chosen.isValid()) {
        setColor(chosen);
    }
}

void ColorButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Dark rounded background
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(45, 45, 45));
    painter.drawRoundedRect(rect(), 4, 4);

    // Inner color circle swatch
    QRect swatch = rect().adjusted(5, 5, -5, -5);
    painter.setPen(QPen(QColor(255, 255, 255, 180), 1.5));
    painter.setBrush(m_color);
    painter.drawEllipse(swatch);
}

