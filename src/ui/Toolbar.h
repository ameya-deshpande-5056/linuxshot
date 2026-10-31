#pragma once

#include <QWidget>
#include <QButtonGroup>
#include <QBoxLayout>
#include <QPushButton>

class Toolbar : public QWidget {
    Q_OBJECT

public:
    enum Orientation {
        Horizontal,
        Vertical
    };

    explicit Toolbar(Orientation orientation = Horizontal, QWidget *parent = nullptr);
    ~Toolbar() override = default;

    QPushButton* addToolButton(int id, const QString &text, const QString &tooltip);
    QPushButton* addActionButton(const QString &text, const QString &tooltip);
    void addWidget(QWidget *widget);
    void addSeparator();

    void setCheckedTool(int id);
    int checkedTool() const;

    void updatePosition(const QRect &selectionRect, const QRect &screenBounds);

signals:
    void toolSelected(int id);
    void actionTriggered(const QString &actionName);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Orientation m_orientation;
    QBoxLayout *m_layout = nullptr;
    QButtonGroup *m_group = nullptr;
};

