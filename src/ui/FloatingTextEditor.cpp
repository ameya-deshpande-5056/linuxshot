#include "FloatingTextEditor.h"
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
    m_textEdit->document()->setDocumentMargin(4);

    connect(m_textEdit->document(), &QTextDocument::contentsChanged,
            this, &FloatingTextEditor::adjustHeightToContent);

    hide();
}

void FloatingTextEditor::startEditing(const QRect &geometry, const QFont &font,
                                      const QColor &color, const QString &initialText)
{
    m_font = font;
    m_color = color;

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

void FloatingTextEditor::commit() {
    if (!isVisible()) return;

    QString t = text();
    QPointF pos = baselinePos();
    QFont f = m_font;
    QColor c = m_color;

    hide();
    m_textEdit->clear();

    if (!t.trimmed().isEmpty()) {
        emit committed(t, pos, f, c);
    }
}

void FloatingTextEditor::cancel() {
    if (!isVisible()) return;

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
    qreal bx = x() + BorderWidth + 4;
    qreal by = y() + HeaderHeight + 4 + fm.ascent();
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
        BorderWidth,
        HeaderHeight,
        qMax(20, width() - 2 * BorderWidth),
        qMax(20, height() - HeaderHeight - BorderWidth)
    );
}

void FloatingTextEditor::adjustHeightToContent() {
    if (!m_textEdit) return;

    int docHeight = static_cast<int>(m_textEdit->document()->size().height());
    int neededHeight = HeaderHeight + docHeight + BorderWidth * 2 + 8;
    if (neededHeight > height()) {
        int maxHeight = parentWidget() ? (parentWidget()->height() - y() - 4) : 800;
        int targetHeight = qMin(neededHeight, qMax(height(), maxHeight));
        resize(width(), targetHeight);
        m_textEdit->setGeometry(
            BorderWidth,
            HeaderHeight,
            qMax(20, width() - 2 * BorderWidth),
            qMax(20, height() - HeaderHeight - BorderWidth)
        );
        update();
    }
}

QRect FloatingTextEditor::headerRect() const {
    return QRect(0, 0, width(), HeaderHeight);
}

bool FloatingTextEditor::isOverBorder(const QPoint &pos) const {
    return pos.x() < BorderWidth ||
           pos.x() >= width() - BorderWidth ||
           pos.y() >= height() - BorderWidth;
}

void FloatingTextEditor::clampToParent() {
    if (!parentWidget()) return;

    QRect pRect = parentWidget()->rect();
    int nx = qBound(2, x(), qMax(2, pRect.width() - width() - 2));
    int ny = qBound(2, y(), qMax(2, pRect.height() - height() - 2));
    move(nx, ny);
}

void FloatingTextEditor::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    // Subtle dark backdrop for contrast against any screenshot content
    p.fillRect(rect(), QColor(20, 20, 24, 180));

    // Cyan drag header
    QRect hRect = headerRect();
    p.fillRect(hRect, QColor(0, 132, 255, 230));

    p.setPen(Qt::white);
    QFont hf("Sans Serif", 8, QFont::Bold);
    p.setFont(hf);
    p.drawText(hRect.adjusted(8, 0, -8, 0), Qt::AlignVCenter | Qt::AlignLeft, "Text (drag to move)");

    // Dashed cyan border around the box
    QPen borderPen(QColor(0, 132, 255), 1, Qt::DashLine);
    p.setPen(borderPen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(rect().adjusted(0, 0, -1, -1));
}

void FloatingTextEditor::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    QPoint pos = event->pos();
    if (pos.y() <= HeaderHeight || isOverBorder(pos)) {
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

    QPoint pos = event->pos();
    if (pos.y() <= HeaderHeight || isOverBorder(pos)) {
        setCursor(Qt::SizeAllCursor);
    } else {
        setCursor(Qt::ArrowCursor);
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
    if (watched == m_textEdit && event->type() == QEvent::KeyPress) {
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
    }
    return QWidget::eventFilter(watched, event);
}

