#pragma once

#include <QWidget>
#include <QImage>
#include <QRect>
#include "Toolbar.h"
#include "ColorButton.h"
#include "AnnotationEditor.h"

class SelectionOverlay : public QWidget {
    Q_OBJECT

public:
    enum Handle {
        None,
        TopLeft,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left,
        Move
    };

    explicit SelectionOverlay(QWidget *parent = nullptr);
    ~SelectionOverlay() override = default;

    void setScreenshot(const QImage &screenshot);
    QRect selectionRect() const { return m_selection; }
    void setSelectionRect(const QRect &rect);

signals:
    void saveRequested(const QImage &image);
    void copyRequested(const QImage &image);
    void uploadRequested(const QImage &image);
    void captureCancelled();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setupToolbars();
    void updateToolbarsPosition();
    Handle handleAt(const QPoint &pos) const;
    void updateCursorForHandle(Handle h);
    QRect handleRect(Handle h) const;
    QList<QRect> allHandleRects() const;
    void finalizeSelectionCrop(bool isNewCapture = true);

    QImage m_fullScreenshot;
    QRect m_selection;
    bool m_hasSelection = false;
    bool m_isCreatingSelection = false;
    bool m_isDraggingHandle = false;

    Handle m_activeHandle = None;
    QPoint m_dragStart;
    QRect m_selectionBeforeDrag;

    // Sub-components
    AnnotationEditor *m_editor = nullptr;
    Toolbar *m_toolBar = nullptr;
    Toolbar *m_actionBar = nullptr;
    ColorButton *m_colorBtn = nullptr;

    QPushButton *m_undoBtn = nullptr;
    bool m_annotationMode = false;
};

