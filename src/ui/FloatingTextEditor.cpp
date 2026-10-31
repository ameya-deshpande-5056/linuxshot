#include "FloatingTextEditor.h"
#include "annotation/TextTool.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTextDocument>

FloatingTextEditor::FloatingTextEditor(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, false);
    setMouseTracking(true);

    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->installEventFilter(this);
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textEdit->setFrameShape(QFrame::NoFrame);
    m_textEdit->document()->setDocumentMargin(DocMargin);

    connect(m_textEdit->document(), &QTextDocument::contentsChanged,
            this, &FloatingTextEditor::adjustHeightToContent);

    hide();
}

void FloatingTextEditor::startEditing(const QRect &geometry, const QFont &font,
                                      const QColor &color, const QString &initialText)
{
    m_font = font;
    m_color = color;
    m_isEditing = true;

    setGeometry(geometry);
    clampToParent();
    updateEditorStyle();

    m_textEdit->setPlainText(initialText);
    adjustHeightToContent();

    show();
    raise();
    m_textEdit->setFocus();
    update();
}

qreal FloatingTextEditor::contentWidth() const {
    return qMax(20, width() - 2 * (BorderMargin + DocMargin));
}

QString FloatingTextEditor::wrappedText() const {
    return TextCommand::wrapText(text(), m_font, contentWidth());
}

void FloatingTextEditor::commit() {
    if (!m_isEditing) return;
    m_isEditing = false;

    qreal wrapW = contentWidth();
    QString t = TextCommand::wrapText(text(), m_font, wrapW);
    QPointF pos = baselinePos();
    QFont f = m_font;
    QColor c = m_color;
    int bWidth = static_cast<int>(wrapW);

    hide();
    m_textEdit->clear();

    if (!t.trimmed().isEmpty()) {
        emit committed(t, pos, f, c, bWidth);
    }
}

void FloatingTextEditor::cancel() {
    if (!m_isEditing) return;
    m_isEditing = false;

    hide();
    m_textEdit->clear();
    emit cancelled();
}

void FloatingTextEditor::setTextColor(const QColor &color) {
    m_color = color;
    updateEditorStyle();
    update();
}

void FloatingTextEditor::setTextFont(const QFont &font) {
    m_font = font;
    updateEditorStyle();
    adjustHeightToContent();
    update();
}

QString FloatingTextEditor::text() const {
    return m_textEdit ? m_textEdit->toPlainText() : QString();
}

QPointF FloatingTextEditor::baselinePos() const {
    QFontMetrics fm(m_font);
    qreal bx = x() + BorderMargin + DocMargin;
    qreal by = y() + BorderMargin + DocMargin + fm.ascent();
    return QPointF(bx, by);
}

void FloatingTextEditor::updateEditorStyle() {
    if (!m_textEdit) return;

    m_textEdit->setFont(m_font);
    m_textEdit->setStyleSheet(QString(
        "QPlainTextEdit {"
        "  background: transparent;"
        "  border: none;"
        "  color: %1;"
        "  selection-background-color: rgba(0, 132, 255, 120);"
        "}"
    ).arg(m_color.name()));

    m_textEdit->setGeometry(
        BorderMargin,
        BorderMargin,
        qMax(20, width() - 2 * BorderMargin),
        qMax(20, height() - 2 * BorderMargin)
    );
}

void FloatingTextEditor::adjustHeightToContent() {
    if (!m_textEdit) return;

    int docHeight = static_cast<int>(m_textEdit->document()->size().height());
    int neededHeight = docHeight + BorderMargin * 2 + 4;
    if (neededHeight > height()) {
        int maxHeight = parentWidget() ? (parentWidget()->height() - y() + BorderMargin) : 800;
        int targetHeight = qMin(neededHeight, qMax(height(), maxHeight));
        resize(width(), targetHeight);
        m_textEdit->setGeometry(
            BorderMargin,
            BorderMargin,
            qMax(20, width() - 2 * BorderMargin),
            qMax(20, height() - 2 * BorderMargin)
        );
        update();
    }
}

bool FloatingTextEditor::isOverBorder(const QPoint &pos) const {
    return pos.x() <= BorderMargin ||
           pos.x() >= width() - BorderMargin ||
           pos.y() <= BorderMargin ||
           pos.y() >= height() - BorderMargin;
}

void FloatingTextEditor::clampToParent() {
    if (!parentWidget()) return;

    QRect pRect = parentWidget()->rect();
    // Allow dragging up to -BorderMargin so text can sit right against the canvas edge (y = 0)
    int minX = -BorderMargin;
    int minY = -BorderMargin;
    int maxX = qMax(minX, pRect.width() - width() + BorderMargin);
    int maxY = qMax(minY, pRect.height() - height() + BorderMargin);
    int nx = qBound(minX, x(), maxX);
    int ny = qBound(minY, y(), maxY);
    move(nx, ny);
}

void FloatingTextEditor::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    // Subtle translucent tint so underlying screenshot content remains fully visible
    p.fillRect(rect(), QColor(0, 132, 255, 12));

    // Dashed cyan border around the box
    QPen borderPen(QColor(0, 132, 255, 220), 1, Qt::DashLine);
    p.setPen(borderPen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(rect().adjusted(0, 0, -1, -1));
}

void FloatingTextEditor::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    if (isOverBorder(event->pos())) {
        m_isDragging = true;
        m_dragStartGlobal = event->globalPosition().toPoint();
        m_widgetStartPos = this->pos();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void FloatingTextEditor::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        QPoint delta = event->globalPosition().toPoint() - m_dragStartGlobal;
        move(m_widgetStartPos + delta);
        clampToParent();
        event->accept();
        return;
    }

    if (isOverBorder(event->pos())) {
        setCursor(Qt::SizeAllCursor);
    } else {
        setCursor(Qt::IBeamCursor);
    }

    QWidget::mouseMoveEvent(event);
}

void FloatingTextEditor::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}

bool FloatingTextEditor::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_textEdit) {
        if (event->type() == QEvent::KeyPress) {
            auto *ke = static_cast<QKeyEvent *>(event);
            if (ke->key() == Qt::Key_Escape) {
                cancel();
                return true;
            }
            if ((ke->modifiers() & Qt::ControlModifier) &&
                (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)) {
                commit();
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            auto *me = static_cast<QMouseEvent *>(event);
            if (me->button() == Qt::LeftButton && (me->modifiers() & Qt::AltModifier)) {
                m_isDragging = true;
                m_dragStartGlobal = me->globalPosition().toPoint();
                m_widgetStartPos = this->pos();
                return true;
            }
        } else if (event->type() == QEvent::MouseMove) {
            auto *me = static_cast<QMouseEvent *>(event);
            if (m_isDragging) {
                QPoint delta = me->globalPosition().toPoint() - m_dragStartGlobal;
                move(m_widgetStartPos + delta);
                clampToParent();
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            auto *me = static_cast<QMouseEvent *>(event);
            if (me->button() == Qt::LeftButton && m_isDragging) {
                m_isDragging = false;
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
