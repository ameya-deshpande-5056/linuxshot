#include "CommandHistory.h"

CommandHistory::CommandHistory(QObject *parent)
    : QObject(parent)
{
}

void CommandHistory::addCommand(std::unique_ptr<AnnotationCommand> command) {
    if (!command) return;
    m_undoStack.append(std::shared_ptr<AnnotationCommand>(std::move(command)));
    m_redoStack.clear();
    emit historyChanged();
}

bool CommandHistory::undo() {
    if (m_undoStack.isEmpty()) return false;
    m_redoStack.append(m_undoStack.takeLast());
    emit historyChanged();
    return true;
}

bool CommandHistory::redo() {
    if (m_redoStack.isEmpty()) return false;
    m_undoStack.append(m_redoStack.takeLast());
    emit historyChanged();
    return true;
}

void CommandHistory::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
    emit historyChanged();
}

bool CommandHistory::canUndo() const {
    return !m_undoStack.isEmpty();
}

bool CommandHistory::canRedo() const {
    return !m_redoStack.isEmpty();
}

int CommandHistory::count() const {
    return m_undoStack.size();
}

const QList<std::shared_ptr<AnnotationCommand>>& CommandHistory::commands() const {
    return m_undoStack;
}

void CommandHistory::renderAll(QPainter &painter) const {
    for (const auto &cmd : m_undoStack) {
        if (cmd) {
            cmd->paint(painter);
        }
    }
}

void CommandHistory::applyAllToImage(QImage &image) const {
    for (const auto &cmd : m_undoStack) {
        if (cmd) {
            cmd->applyToImage(image);
        }
    }
}

