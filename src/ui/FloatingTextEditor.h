#pragma once

#include <QWidget>
#include <QPlainTextEdit>
#include <QPointF>
#include <QFont>
#include <QColor>

class FloatingTextEditor : public QWidget {
    Q_OBJECT

public:
    explicit FloatingTextEditor(QWidget *parent = nullptr);
    ~FloatingTextEditor() override = default;

    void startEditing(const QRect &geometry, const QFont &font, const QColor &color, const QString &initialText = QString());
    void commit();
    void cancel();

    void setTextColor(const QColor &color);
    void setTextFont(const QFont &font);

    QString text() const;
    QPointF baselinePos() const;
    QFont currentFont() const { return m_font; }
    QColor currentColor() const { return m_color; }
    bool isEditing() const { return m_isEditing; }

signals:
    void committed(const QString &text, const QPointF &baselinePos, const QFont &font, const QColor &color);
    void cancelled();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void updateEditorStyle();
    void adjustHeightToContent();
    bool isOverBorder(const QPoint &pos) const;
    void clampToParent();

    QPlainTextEdit *m_textEdit = nullptr;
    QFont m_font;
    QColor m_color = Qt::red;
    bool m_isEditing = false;

    bool m_isDragging = false;
    QPoint m_dragStartGlobal;
    QPoint m_widgetStartPos;

    static constexpr int BorderMargin = 5;
    static constexpr int DocMargin = 2;
};

