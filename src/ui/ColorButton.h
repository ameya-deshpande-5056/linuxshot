#pragma once

#include <QPushButton>
#include <QColor>

class ColorButton : public QPushButton {
    Q_OBJECT

public:
    explicit ColorButton(QWidget *parent = nullptr);
    explicit ColorButton(const QColor &color, QWidget *parent = nullptr);

    QColor color() const { return m_color; }
    void setColor(const QColor &color);

signals:
    void colorChanged(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void chooseColor();

private:
    QColor m_color = Qt::red;
};
