#pragma once

#include <QObject>
#include <QList>
#include <memory>
#include "AnnotationCommand.h"

class CommandHistory : public QObject {
    Q_OBJECT

public:
    explicit CommandHistory(QObject *parent = nullptr);
    ~CommandHistory() override = default;

    void addCommand(std::unique_ptr<AnnotationCommand> command);
    bool undo();
    bool redo();
    void clear();

    bool canUndo() const;
    bool canRedo() const;
    int count() const;

    const QList<std::shared_ptr<AnnotationCommand>>& commands() const;

    void renderAll(QPainter &painter) const;
    void applyAllToImage(QImage &image) const;

signals:
    void historyChanged();

private:
    QList<std::shared_ptr<AnnotationCommand>> m_undoStack;
    QList<std::shared_ptr<AnnotationCommand>> m_redoStack;
};

