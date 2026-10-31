#include "Toolbar.h"
#include <QPainter>
#include <QFrame>

Toolbar::Toolbar(Orientation orientation, QWidget *parent)
    : QWidget(parent), m_orientation(orientation)
{
    setWindowFlags(Qt::SubWindow);
    setAttribute(Qt::WA_TranslucentBackground, false);

    m_group = new QButtonGroup(this);
    m_group->setExclusive(true);

    if (orientation == Horizontal) {
        m_layout = new QHBoxLayout(this);
    } else {
        m_layout = new QVBoxLayout(this);
    }

    m_layout->setContentsMargins(4, 4, 4, 4);
    m_layout->setSpacing(4);

    connect(m_group, &QButtonGroup::idClicked, this, &Toolbar::toolSelected);

    setStyleSheet(
        "QWidget { background-color: #2D2D2D; border-radius: 5px; }"
        "QPushButton {"
        "   background-color: transparent;"
        "   color: #FFFFFF;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "   border: 1px solid transparent;"
        "   border-radius: 4px;"
        "   min-width: 26px;"
        "   min-height: 26px;"
        "   max-width: 26px;"
        "   max-height: 26px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3F3F3F;"
        "   border: 1px solid #555555;"
        "}"
        "QPushButton:checked {"
        "   background-color: #1E66D5;"
        "   border: 1px solid #60A5FA;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #154A99;"
        "}"
    );
}

QPushButton* Toolbar::addToolButton(int id, const QString &text, const QString &tooltip) {
    auto *btn = new QPushButton(text, this);
    btn->setCheckable(true);
    btn->setToolTip(tooltip);
    btn->setCursor(Qt::PointingHandCursor);

    m_group->addButton(btn, id);
    m_layout->addWidget(btn);
    return btn;
}

QPushButton* Toolbar::addActionButton(const QString &text, const QString &tooltip) {
    auto *btn = new QPushButton(text, this);
    btn->setCheckable(false);
    btn->setToolTip(tooltip);
    btn->setCursor(Qt::PointingHandCursor);

    connect(btn, &QPushButton::clicked, this, [this, tooltip]() {
        emit actionTriggered(tooltip);
    });

    m_layout->addWidget(btn);
    return btn;
}

void Toolbar::addWidget(QWidget *widget) {
    if (widget) {
        m_layout->addWidget(widget);
    }
}

void Toolbar::addSeparator() {
    auto *line = new QFrame(this);
    if (m_orientation == Horizontal) {
        line->setFrameShape(QFrame::VLine);
        line->setFixedWidth(2);
    } else {
        line->setFrameShape(QFrame::HLine);
        line->setFixedHeight(2);
    }
    line->setStyleSheet("background-color: #555555;");
    m_layout->addWidget(line);
}

void Toolbar::setCheckedTool(int id) {
    if (auto *btn = m_group->button(id)) {
        btn->setChecked(true);
    }
}

int Toolbar::checkedTool() const {
    return m_group->checkedId();
}

void Toolbar::updatePosition(const QRect &selectionRect, const QRect &screenBounds) {
    QSize sz = size();
    if (sz.width() < 10 || sz.height() < 10) {
        adjustSize();
        sz = size();
    }

    if (m_orientation == Horizontal) {
        // Bottom toolbar: placed below selection; if off screen, place above
        int x = selectionRect.right() - sz.width();
        int y = selectionRect.bottom() + 6;

        if (y + sz.height() > screenBounds.bottom()) {
            y = selectionRect.top() - sz.height() - 6;
        }
        x = qBound(screenBounds.left() + 4, x, screenBounds.right() - sz.width() - 4);
        y = qBound(screenBounds.top() + 4, y, screenBounds.bottom() - sz.height() - 4);

        move(x, y);
    } else {
        // Vertical toolbar: placed to right of selection; if off screen, place inside or to left
        int x = selectionRect.right() + 6;
        int y = selectionRect.top();

        if (x + sz.width() > screenBounds.right()) {
            x = selectionRect.left() - sz.width() - 6;
            if (x < screenBounds.left()) {
                x = selectionRect.right() - sz.width() - 6;
            }
        }
        x = qBound(screenBounds.left() + 4, x, screenBounds.right() - sz.width() - 4);
        y = qBound(screenBounds.top() + 4, y, screenBounds.bottom() - sz.height() - 4);

        move(x, y);
    }
}

void Toolbar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Border around toolbar
    p.setPen(QColor(80, 80, 80, 200));
    p.setBrush(QColor(45, 45, 45, 240));
    p.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 5, 5);
}

