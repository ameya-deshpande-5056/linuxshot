#include "AnnotationEditor.h"
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
#include <QInputDialog>

AnnotationEditor::AnnotationEditor(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setMouseTracking(true);
    initTools();

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

void AnnotationEditor::setBaseImage(const QImage &image) {
    m_baseImage = image;
    m_history.clear();
    setFixedSize(image.size());
    update();
}

void AnnotationEditor::updateCrop(const QRect &geometry, const QImage &image) {
    m_baseImage = image;
    setGeometry(geometry);
    setFixedSize(image.size());
    update();
}

QImage AnnotationEditor::getRenderedImage() const {
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
    update();
}

int AnnotationEditor::strokeWidth() const {
    return m_activeWidth;
}

void AnnotationEditor::undo() {
    m_history.undo();
}

void AnnotationEditor::redo() {
    m_history.redo();
}

void AnnotationEditor::clear() {
    m_history.clear();
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
    if (m_tools.contains(m_activeToolType)) {
        m_tools[m_activeToolType]->paintPreview(p);
    }
}

void AnnotationEditor::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    if (m_activeToolType == ToolType::Text) {
        bool ok = false;
        QString text = QInputDialog::getText(
            this, "Add Text", "Enter text annotation:",
            QLineEdit::Normal, "", &ok
        );
        if (ok && !text.trimmed().isEmpty()) {
            auto *textTool = static_cast<TextTool *>(m_tools[ToolType::Text].get());
            textTool->mousePress(event->position());
            textTool->setText(text);
            auto cmd = textTool->createCommand();
            if (cmd) {
                m_history.addCommand(std::move(cmd));
                emit imageModified();
            }
        }
        return;
    }

    if (m_tools.contains(m_activeToolType)) {
        m_tools[m_activeToolType]->mousePress(event->position());
        update();
    }
}

void AnnotationEditor::mouseMoveEvent(QMouseEvent *event) {
    if (m_tools.contains(m_activeToolType) && m_tools[m_activeToolType]->isDrawing()) {
        m_tools[m_activeToolType]->mouseMove(event->position());
        update();
    }
}

void AnnotationEditor::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

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
