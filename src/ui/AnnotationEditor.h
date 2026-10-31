#pragma once

#include <QWidget>
#include <QImage>
#include <memory>
#include <QMap>
#include "annotation/AnnotationTool.h"
#include "annotation/CommandHistory.h"

class AnnotationEditor : public QWidget {
    Q_OBJECT

public:
    explicit AnnotationEditor(QWidget *parent = nullptr);
    ~AnnotationEditor() override = default;

    void setBaseImage(const QImage &image);
    QImage getRenderedImage() const;

    void setActiveTool(ToolType type);
    ToolType activeTool() const;

    void setColor(const QColor &color);
    QColor color() const;

    void setStrokeWidth(int width);
    int strokeWidth() const;

    void undo();
    void redo();
    void clear();

    bool canUndo() const;
    bool canRedo() const;

signals:
    void historyChanged();
    void imageModified();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void initTools();

    QImage m_baseImage;
    CommandHistory m_history;
    QMap<ToolType, std::shared_ptr<AnnotationTool>> m_tools;
    ToolType m_activeToolType = ToolType::Pen;
    QColor m_activeColor = Qt::red;
    int m_activeWidth = 3;
};
