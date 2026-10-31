#include "AnnotationEditor.h"
#include "FloatingTextEditor.h"
#include <QMouseEvent>
#include "annotation/PenTool.h"
#include "annotation/LineTool.h"
#include "annotation/ArrowTool.h"
#include "annotation/RectTool.h"
#include "annotation/CircleTool.h"
#include "annotation/MarkerTool.h"
#include "annotation/TextTool.h"
#include "annotation/BlurTool.h"
#include <QPainter>

AnnotationEditor::AnnotationEditor(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setMouseTracking(true);
    initTools();

    m_floatingTextEditor = new FloatingTextEditor(this);
    connect(m_floatingTextEditor, &FloatingTextEditor::committed, this,
            [this](const QString &text, const QPointF &pos, const QFont &font, const QColor &color) {
        if (!text.trimmed().isEmpty()) {
            auto cmd = std::make_unique<TextCommand>(text, pos, font, color);
            m_history.addCommand(std::move(cmd));
            emit imageModified();
            update();
        }
    });

    connect(&m_history, &CommandHistory::historyChanged, this, [this]() {
        update();
        emit historyChanged();
    });
}

void AnnotationEditor::initTools() {
    m_tools[ToolType::Pen]       = std::make_shared<PenTool>();
    m_tools[ToolType::Line]      = std::make_shared<LineTool>();
    m_tools[ToolType::Arrow]     = std::make_shared<ArrowTool>();
    m_tools[ToolType::Rectangle] = std::make_shared<RectTool>();
    m_tools[ToolType::Circle]    = std::make_shared<CircleTool>();
    m_tools[ToolType::Marker]    = std::make_shared<MarkerTool>();
    m_tools[ToolType::Text]      = std::make_shared<TextTool>();
    m_tools[ToolType::Blur]      = std::make_shared<BlurTool>();

    setColor(m_activeColor);
    setStrokeWidth(m_activeWidth);
}

void AnnotationEditor::commitFloatingText() {
    if (m_floatingTextEditor && m_floatingTextEditor->isVisible()) {
        m_floatingTextEditor->commit();
    }
}

void AnnotationEditor::setBaseImage(const QImage &image) {
    commitFloatingText();
    m_baseImage = image;
    m_history.clear();
    setFixedSize(image.size());
    update();
}

void AnnotationEditor::updateCrop(const QRect &geometry, const QImage &image) {
    commitFloatingText();
    m_baseImage = image;
    setGeometry(geometry);
    setFixedSize(image.size());
    update();
}

QImage AnnotationEditor::getRenderedImage() const {
    const_cast<AnnotationEditor*>(this)->commitFloatingText();
    if (m_baseImage.isNull()) return QImage();

    QImage result = m_baseImage.copy();
    // Apply blur pixelations directly to pixel buffer first
    for (const auto &cmd : m_history.commands()) {
        if (auto *blur = dynamic_cast<const BlurCommand *>(cmd.get())) {
            blur->applyToImage(result);
        }
    }

    // Paint vector annotations
    QPainter p(&result);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    for (const auto &cmd : m_history.commands()) {
        if (!dynamic_cast<const BlurCommand *>(cmd.get())) {
            cmd->paint(p);
        }
    }
    p.end();

    return result;
}

void AnnotationEditor::setActiveTool(ToolType type) {
    if (m_activeToolType != type) {
        commitFloatingText();
        if (m_tools.contains(m_activeToolType)) {
            m_tools[m_activeToolType]->reset();
        }
        m_activeToolType = type;
        update();
    }
}

ToolType AnnotationEditor::activeTool() const {
    return m_activeToolType;
}

void AnnotationEditor::setColor(const QColor &color) {
    m_activeColor = color;
    for (auto &tool : m_tools) {
        tool->setColor(color);
    }
    if (m_floatingTextEditor) {
        m_floatingTextEditor->setTextColor(color);
    }
    update();
}

QColor AnnotationEditor::color() const {
    return m_activeColor;
}

void AnnotationEditor::setStrokeWidth(int width) {
    m_activeWidth = qBound(1, width, 50);
    for (auto &tool : m_tools) {
        if (tool->type() != ToolType::Marker) {
            tool->setWidth(m_activeWidth);
        }
    }
    if (m_floatingTextEditor) {
        int fontSize = qBound(12, 10 + m_activeWidth * 2, 48);
        m_floatingTextEditor->setTextFont(QFont("Sans Serif", fontSize, QFont::Bold));
    }
    update();
}

int AnnotationEditor::strokeWidth() const {
    return m_activeWidth;
}

void AnnotationEditor::undo() {
    commitFloatingText();
    m_history.undo();
}

void AnnotationEditor::redo() {
    commitFloatingText();
    m_history.redo();
}

void AnnotationEditor::clear() {
    commitFloatingText();
    m_history.clear();
}

void AnnotationEditor::addCommand(std::unique_ptr<AnnotationCommand> cmd) {
    if (cmd) {
        m_history.addCommand(std::move(cmd));
        update();
        emit imageModified();
    }
}

bool AnnotationEditor::canUndo() const {
    return m_history.canUndo();
}

bool AnnotationEditor::canRedo() const {
    return m_history.canRedo();
}

void AnnotationEditor::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    // Draw base image
    if (!m_baseImage.isNull()) {
        p.drawImage(0, 0, m_baseImage);
    }

    // Draw all commands from history
    m_history.renderAll(p);

    // Draw in-progress preview of active tool
    if (m_tools.contains(m_activeToolType) && m_activeToolType != ToolType::Text) {
        m_tools[m_activeToolType]->paintPreview(p);
    }

    // Draw text box rubberband preview while dragging to draw text box
    if (m_isDrawingTextRect && !m_textPreviewRect.isEmpty()) {
        p.save();
        p.setPen(QPen(QColor(0, 132, 255), 1, Qt::DashLine));
        p.setBrush(QColor(0, 132, 255, 30));
        p.drawRect(m_textPreviewRect);
        p.restore();
    }
}

void AnnotationEditor::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    // If floating text editor is currently active:
    if (m_floatingTextEditor && m_floatingTextEditor->isVisible()) {
        QPoint pos = event->pos();
        if (!m_floatingTextEditor->geometry().contains(pos)) {
            // Click outside commits the textbox and makes it permanent!
            commitFloatingText();
            // Consumed: Do not spawn a new textbox on the same click that committed the previous one
            return;
        }
    }

    if (m_activeToolType == ToolType::Text) {
        m_isDrawingTextRect = true;
        m_textDragStart = event->pos();
        m_textPreviewRect = QRect(m_textDragStart, m_textDragStart);
        return;
    }

    if (m_tools.contains(m_activeToolType)) {
        m_tools[m_activeToolType]->mousePress(event->position());
        update();
    }
}

void AnnotationEditor::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDrawingTextRect) {
        m_textPreviewRect = QRect(m_textDragStart, event->pos()).normalized();
        update();
        return;
    }

    if (m_tools.contains(m_activeToolType) && m_tools[m_activeToolType]->isDrawing()) {
        m_tools[m_activeToolType]->mouseMove(event->position());
        update();
    }
}

void AnnotationEditor::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    if (m_isDrawingTextRect) {
        m_isDrawingTextRect = false;
        QRect boxRect = QRect(m_textDragStart, event->pos()).normalized();

        // If user just clicked or made a very small drag, provide a comfortable default size
        if (boxRect.width() < 60 || boxRect.height() < 30) {
            int defaultW = 180;
            int defaultH = 56;
            boxRect = QRect(m_textDragStart, QSize(defaultW, defaultH));
        }

        // Clamp inside editor geometry
        if (boxRect.right() >= width()) boxRect.moveRight(width() - 2);
        if (boxRect.bottom() >= height()) boxRect.moveBottom(height() - 2);
        if (boxRect.left() < 0) boxRect.moveLeft(2);
        if (boxRect.top() < 0) boxRect.moveTop(2);

        m_textPreviewRect = QRect();

        int fontSize = qBound(12, 10 + m_activeWidth * 2, 48);
        QFont f("Sans Serif", fontSize, QFont::Bold);
        m_floatingTextEditor->startEditing(boxRect, f, m_activeColor);
        update();
        return;
    }

    if (m_tools.contains(m_activeToolType) && m_tools[m_activeToolType]->isDrawing()) {
        m_tools[m_activeToolType]->mouseRelease(event->position());

        auto cmd = m_tools[m_activeToolType]->createCommand();
        if (cmd) {
            // For blur command, cache the pixelation from the current base image
            if (auto *blur = dynamic_cast<BlurCommand *>(cmd.get())) {
                blur->cachePixelation(m_baseImage);
            }
            m_history.addCommand(std::move(cmd));
            emit imageModified();
        }
        update();
    }
}
