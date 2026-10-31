#include "SelectionOverlay.h"
#include "IconHelper.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>

SelectionOverlay::SelectionOverlay(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_StaticContents, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_editor = new AnnotationEditor(this);
    m_editor->installEventFilter(this);
    m_editor->hide();

    setupToolbars();
}

void SelectionOverlay::setupToolbars() {
    // 1. Vertical Tool Toolbar (Annotation Tools)
    m_toolBar = new Toolbar(Toolbar::Vertical, this);
    m_toolBar->addToolButton(static_cast<int>(ToolType::Pen),       IconHelper::toolIcon(ToolType::Pen),       "Pen");
    m_toolBar->addToolButton(static_cast<int>(ToolType::Line),      IconHelper::toolIcon(ToolType::Line),      "Line");
    m_toolBar->addToolButton(static_cast<int>(ToolType::Arrow),     IconHelper::toolIcon(ToolType::Arrow),     "Arrow");
    m_toolBar->addToolButton(static_cast<int>(ToolType::Rectangle), IconHelper::toolIcon(ToolType::Rectangle), "Rectangle");
    m_toolBar->addToolButton(static_cast<int>(ToolType::Circle),    IconHelper::toolIcon(ToolType::Circle),    "Circle");
    m_toolBar->addToolButton(static_cast<int>(ToolType::Marker),    IconHelper::toolIcon(ToolType::Marker),    "Marker");
    m_toolBar->addToolButton(static_cast<int>(ToolType::Text),      IconHelper::toolIcon(ToolType::Text),      "Text");
    m_toolBar->addToolButton(static_cast<int>(ToolType::Blur),      IconHelper::toolIcon(ToolType::Blur),      "Blur / Pixelate");

    m_toolBar->addSeparator();

    m_colorBtn = new ColorButton(Qt::red, m_toolBar);
    m_toolBar->addWidget(m_colorBtn);

    m_undoBtn = m_toolBar->addActionButton(IconHelper::actionIcon(IconHelper::Action::Undo), "Undo", "Undo (Ctrl+Z)");
    m_undoBtn->setEnabled(false);

    m_toolBar->setCheckedTool(static_cast<int>(ToolType::Pen));

    connect(m_toolBar, &Toolbar::toolSelected, this, [this](int id) {
        m_editor->setActiveTool(static_cast<ToolType>(id));
    });

    connect(m_colorBtn, &ColorButton::colorChanged, this, [this](const QColor &c) {
        m_editor->setColor(c);
    });

    connect(m_toolBar, &Toolbar::actionTriggered, this, [this](const QString &action) {
        if (action.startsWith("Undo")) {
            m_editor->undo();
        }
    });

    connect(m_editor, &AnnotationEditor::historyChanged, this, [this]() {
        if (m_undoBtn) {
            m_undoBtn->setEnabled(m_editor->canUndo());
        }
    });

    // 2. Horizontal Action Toolbar (Save, Copy, Upload, Close)
    m_actionBar = new Toolbar(Toolbar::Horizontal, this);
    m_actionBar->addActionButton(IconHelper::actionIcon(IconHelper::Action::Upload), "Upload", "Upload (Ctrl+D)");
    m_actionBar->addActionButton(IconHelper::actionIcon(IconHelper::Action::Copy),   "Copy",   "Copy (Ctrl+C)");
    m_actionBar->addActionButton(IconHelper::actionIcon(IconHelper::Action::Save),   "Save",   "Save (Ctrl+S)");
    m_actionBar->addSeparator();
    m_actionBar->addActionButton(IconHelper::actionIcon(IconHelper::Action::Cancel), "Cancel", "Cancel (Esc)");

    connect(m_actionBar, &Toolbar::actionTriggered, this, [this](const QString &action) {
        QImage finalImg = m_editor->isVisible() ? m_editor->getRenderedImage()
                                                : m_fullScreenshot.copy(m_selection);
        hide();
        close();
        QGuiApplication::processEvents();

        if (action == "Upload") {
            emit uploadRequested(finalImg);
        } else if (action == "Copy") {
            emit copyRequested(finalImg);
        } else if (action == "Save") {
            emit saveRequested(finalImg);
        } else if (action == "Cancel") {
            emit captureCancelled();
        }
    });

    m_toolBar->hide();
    m_actionBar->hide();
}

void SelectionOverlay::setScreenshot(const QImage &screenshot) {
    m_fullScreenshot = screenshot;
    m_hasSelection = false;
    m_selection = QRect();
    m_editor->hide();
    m_toolBar->hide();
    m_actionBar->hide();

    // Span full virtual desktop across all connected monitors
    QRect virtualDesktop;
    const auto screens = QGuiApplication::screens();
    for (QScreen *s : screens) {
        virtualDesktop = virtualDesktop.united(s->geometry());
    }
    setGeometry(virtualDesktop);
    update();
}

void SelectionOverlay::setSelectionRect(const QRect &rect) {
    m_selection = rect.normalized().intersected(this->rect());
    m_hasSelection = !m_selection.isEmpty();
    if (m_hasSelection) {
        finalizeSelectionCrop();
    }
    update();
}

void SelectionOverlay::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    setFocus();
    activateWindow();
    raise();
}

QRect SelectionOverlay::handleRect(Handle h) const {
    const int s = 8;
    const int half = s / 2;

    switch (h) {
        case TopLeft:     return QRect(m_selection.left() - half, m_selection.top() - half, s, s);
        case Top:         return QRect(m_selection.center().x() - half, m_selection.top() - half, s, s);
        case TopRight:    return QRect(m_selection.right() - half, m_selection.top() - half, s, s);
        case Right:       return QRect(m_selection.right() - half, m_selection.center().y() - half, s, s);
        case BottomRight: return QRect(m_selection.right() - half, m_selection.bottom() - half, s, s);
        case Bottom:      return QRect(m_selection.center().x() - half, m_selection.bottom() - half, s, s);
        case BottomLeft:  return QRect(m_selection.left() - half, m_selection.bottom() - half, s, s);
        case Left:        return QRect(m_selection.left() - half, m_selection.center().y() - half, s, s);
        default:          return QRect();
    }
}

QList<QRect> SelectionOverlay::allHandleRects() const {
    return {
        handleRect(TopLeft), handleRect(Top), handleRect(TopRight),
        handleRect(Right), handleRect(BottomRight), handleRect(Bottom),
        handleRect(BottomLeft), handleRect(Left)
    };
}

SelectionOverlay::Handle SelectionOverlay::handleAt(const QPoint &pos) const {
    if (!m_hasSelection) return None;

    auto hitTest = [this, &pos](Handle h) {
        return handleRect(h).adjusted(-3, -3, 3, 3).contains(pos);
    };

    if (hitTest(TopLeft))     return TopLeft;
    if (hitTest(TopRight))    return TopRight;
    if (hitTest(BottomRight)) return BottomRight;
    if (hitTest(BottomLeft))  return BottomLeft;
    if (hitTest(Top))         return Top;
    if (hitTest(Bottom))      return Bottom;
    if (hitTest(Left))        return Left;
    if (hitTest(Right))       return Right;

    if (m_selection.contains(pos)) {
        return Move;
    }
    return None;
}

void SelectionOverlay::updateCursorForHandle(Handle h) {
    switch (h) {
        case TopLeft:
        case BottomRight: setCursor(Qt::SizeFDiagCursor); break;
        case TopRight:
        case BottomLeft:  setCursor(Qt::SizeBDiagCursor); break;
        case Top:
        case Bottom:      setCursor(Qt::SizeVerCursor); break;
        case Left:
        case Right:       setCursor(Qt::SizeHorCursor); break;
        case Move:        setCursor(Qt::SizeAllCursor); break;
        default:          setCursor(Qt::CrossCursor); break;
    }
}

void SelectionOverlay::updateToolbarsPosition() {
    if (!m_hasSelection) return;

    QRect bounds = rect();
    m_toolBar->updatePosition(m_selection, bounds);
    m_actionBar->updatePosition(m_selection, bounds);
}

void SelectionOverlay::finalizeSelectionCrop(bool isNewCapture) {
    if (m_selection.width() < 10 || m_selection.height() < 10) return;

    QImage cropped = m_fullScreenshot.copy(m_selection);
    if (isNewCapture) {
        m_editor->setBaseImage(cropped);
    } else {
        m_editor->updateCrop(m_selection, cropped);
    }
    m_editor->setGeometry(m_selection);
    m_editor->show();
    m_editor->raise();

    m_toolBar->show();
    m_toolBar->raise();
    m_actionBar->show();
    m_actionBar->raise();
    updateToolbarsPosition();
}

void SelectionOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    // 1. Draw base desktop capture
    if (!m_fullScreenshot.isNull()) {
        p.drawImage(0, 0, m_fullScreenshot);
    }

    // 2. Draw darkened scrim over unselected area
    p.setBrush(QColor(0, 0, 0, 120));
    p.setPen(Qt::NoPen);

    if (!m_hasSelection) {
        p.drawRect(rect());

        // Draw initial hint
        p.setPen(Qt::white);
        p.setFont(QFont("Sans Serif", 14, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "Select an area to capture\n(Esc to Cancel)");
        return;
    }

    // Darken 4 surrounding regions around selection rectangle
    QRect r = m_selection;
    p.drawRect(0, 0, width(), r.top());                                    // top
    p.drawRect(0, r.bottom() + 1, width(), height() - r.bottom() - 1);       // bottom
    p.drawRect(0, r.top(), r.left(), r.height());                          // left
    p.drawRect(r.right() + 1, r.top(), width() - r.right() - 1, r.height());// right

    // 3. Selection border (cyan dashed line)
    QPen borderPen(QColor(0, 132, 255), 1, Qt::DashLine);
    p.setPen(borderPen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(r.adjusted(0, 0, -1, -1));

    // 4. Resize handles
    p.setPen(QColor(0, 100, 200));
    p.setBrush(QColor(255, 255, 255));
    for (const QRect &hr : allHandleRects()) {
        p.drawRect(hr);
    }

    // 5. Dimension badge (e.g. "800 x 600")
    QString dimText = QString("%1 × %2 px").arg(r.width()).arg(r.height());
    QFont dimFont("Sans Serif", 9, QFont::Bold);
    QFontMetrics fm(dimFont);
    int badgeW = fm.horizontalAdvance(dimText) + 12;
    int badgeH = fm.height() + 6;

    int badgeY = r.top() - badgeH - 4;
    if (badgeY < 4) badgeY = r.top() + 4;
    int badgeX = qBound(4, r.left(), width() - badgeW - 4);

    QRect badgeRect(badgeX, badgeY, badgeW, badgeH);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(30, 30, 30, 220));
    p.drawRoundedRect(badgeRect, 3, 3);

    p.setPen(Qt::white);
    p.setFont(dimFont);
    p.drawText(badgeRect, Qt::AlignCenter, dimText);
}

void SelectionOverlay::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    QPoint pos = event->pos();
    Handle h = handleAt(pos);

    if (m_hasSelection && h != None) {
        m_activeHandle = h;
        m_isDraggingHandle = true;
        m_dragStart = pos;
        m_selectionBeforeDrag = m_selection;
        m_editor->hide();
        m_toolBar->show();
        m_toolBar->raise();
        m_actionBar->show();
        m_actionBar->raise();
        grabMouse();
    } else {
        // Start creating a brand new selection
        m_isCreatingSelection = true;
        m_hasSelection = false;
        m_dragStart = pos;
        m_selection = QRect(pos, pos);
        m_editor->hide();
        m_toolBar->hide();
        m_actionBar->hide();
    }
    update();
}

void SelectionOverlay::mouseMoveEvent(QMouseEvent *event) {
    QPoint pos = event->pos();

    if (m_isCreatingSelection) {
        m_selection = QRect(m_dragStart, pos).normalized().intersected(rect());
        m_hasSelection = (m_selection.width() > 2 && m_selection.height() > 2);
        update();
        return;
    }

    if (m_isDraggingHandle) {
        QPoint delta = pos - m_dragStart;
        QRect r = m_selectionBeforeDrag;

        switch (m_activeHandle) {
            case TopLeft:     r.setTopLeft(r.topLeft() + delta); break;
            case Top:         r.setTop(r.top() + delta.y()); break;
            case TopRight:    r.setTopRight(r.topRight() + delta); break;
            case Right:       r.setRight(r.right() + delta.x()); break;
            case BottomRight: r.setBottomRight(r.bottomRight() + delta); break;
            case Bottom:      r.setBottom(r.bottom() + delta.y()); break;
            case BottomLeft:  r.setBottomLeft(r.bottomLeft() + delta); break;
            case Left:        r.setLeft(r.left() + delta.x()); break;
            case Move: {
                r.translate(delta);
                if (r.left() < 0) r.moveLeft(0);
                if (r.top() < 0) r.moveTop(0);
                if (r.right() >= width()) r.moveRight(width() - 1);
                if (r.bottom() >= height()) r.moveBottom(height() - 1);
                break;
            }
            default: break;
        }

        if (m_activeHandle != Move) {
            m_selection = r.normalized().intersected(rect());
        } else {
            m_selection = r;
        }

        if (m_selection.width() >= 10 && m_selection.height() >= 10) {
            updateToolbarsPosition();
            m_toolBar->show();
            m_toolBar->raise();
            m_actionBar->show();
            m_actionBar->raise();
        }
        update();
        return;
    }

    // Hover cursor updates
    Handle h = handleAt(pos);
    updateCursorForHandle(h);
}

void SelectionOverlay::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    if (m_isCreatingSelection) {
        m_isCreatingSelection = false;
        if (m_selection.width() >= 10 && m_selection.height() >= 10) {
            m_hasSelection = true;
            finalizeSelectionCrop(true);
        } else {
            m_hasSelection = false;
            m_selection = QRect();
            m_toolBar->hide();
            m_actionBar->hide();
        }
    } else if (m_isDraggingHandle) {
        if (mouseGrabber() == this) {
            releaseMouse();
        }
        m_isDraggingHandle = false;
        m_activeHandle = None;
        if (m_selection.width() >= 10 && m_selection.height() >= 10) {
            finalizeSelectionCrop(false);
        }
    }

    update();
}

bool SelectionOverlay::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_editor) {
        if (event->type() == QEvent::MouseMove) {
            auto *me = static_cast<QMouseEvent *>(event);
            QPoint pos = m_editor->mapTo(this, me->pos());
            Handle h = handleAt(pos);
            if (h != None && h != Move) {
                updateCursorForHandle(h);
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            auto *me = static_cast<QMouseEvent *>(event);
            if (me->button() == Qt::LeftButton) {
                QPoint pos = m_editor->mapTo(this, me->pos());
                Handle h = handleAt(pos);
                if (h != None && h != Move) {
                    m_activeHandle = h;
                    m_isDraggingHandle = true;
                    m_dragStart = pos;
                    m_selectionBeforeDrag = m_selection;
                    m_editor->hide();
                    m_toolBar->show();
                    m_toolBar->raise();
                    m_actionBar->show();
                    m_actionBar->raise();
                    grabMouse();
                    update();
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void SelectionOverlay::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        hide();
        close();
        QGuiApplication::processEvents();
        emit captureCancelled();
        return;
    }

    if (event->matches(QKeySequence::SelectAll)) { // Ctrl+A
        m_selection = rect();
        m_hasSelection = true;
        finalizeSelectionCrop();
        update();
        return;
    }

    if (event->matches(QKeySequence::Copy)) { // Ctrl+C
        if (m_hasSelection) {
            QImage img = m_editor->isVisible() ? m_editor->getRenderedImage()
                                               : m_fullScreenshot.copy(m_selection);
            hide();
            close();
            QGuiApplication::processEvents();
            emit copyRequested(img);
        }
        return;
    }

    if (event->matches(QKeySequence::Save)) { // Ctrl+S
        if (m_hasSelection) {
            QImage img = m_editor->isVisible() ? m_editor->getRenderedImage()
                                               : m_fullScreenshot.copy(m_selection);
            hide();
            close();
            QGuiApplication::processEvents();
            emit saveRequested(img);
        }
        return;
    }

    if (event->matches(QKeySequence::Undo)) { // Ctrl+Z
        if (m_editor->isVisible()) {
            m_editor->undo();
        }
        return;
    }

    // Arrow keys nudge selection
    if (m_hasSelection && !m_isCreatingSelection && !m_isDraggingHandle) {
        int delta = (event->modifiers() & Qt::ShiftModifier) ? 10 : 1;
        QRect r = m_selection;

        switch (event->key()) {
            case Qt::Key_Left:  r.translate(-delta, 0); break;
            case Qt::Key_Right: r.translate(delta, 0); break;
            case Qt::Key_Up:    r.translate(0, -delta); break;
            case Qt::Key_Down:  r.translate(0, delta); break;
            default:
                QWidget::keyPressEvent(event);
                return;
        }

        m_selection = r.intersected(rect());
        finalizeSelectionCrop();
        update();
        return;
    }

    QWidget::keyPressEvent(event);
}

