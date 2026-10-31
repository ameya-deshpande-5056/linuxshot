#include <QTest>
#include "annotation/CommandHistory.h"
#include "annotation/AnnotationCommand.h"

class DummyCommand : public AnnotationCommand {
public:
    explicit DummyCommand(int id) : m_id(id) {}
    void paint(QPainter &) const override {}
    QString description() const override { return QString("Dummy %1").arg(m_id); }
    std::unique_ptr<AnnotationCommand> clone() const override {
        return std::make_unique<DummyCommand>(m_id);
    }
    int id() const { return m_id; }
private:
    int m_id;
};

class TestCommandHistory : public QObject {
    Q_OBJECT

private slots:
    void testInitialState();
    void testAddCommand();
    void testUndoRedo();
    void testClear();
    void testRedoInvalidationOnNewCommand();
};

void TestCommandHistory::testInitialState() {
    CommandHistory history;
    QCOMPARE(history.count(), 0);
    QCOMPARE(history.canUndo(), false);
    QCOMPARE(history.canRedo(), false);
}

void TestCommandHistory::testAddCommand() {
    CommandHistory history;
    history.addCommand(std::make_unique<DummyCommand>(1));
    QCOMPARE(history.count(), 1);
    QCOMPARE(history.canUndo(), true);
    QCOMPARE(history.canRedo(), false);

    history.addCommand(std::make_unique<DummyCommand>(2));
    QCOMPARE(history.count(), 2);
    QCOMPARE(history.canUndo(), true);
}

void TestCommandHistory::testUndoRedo() {
    CommandHistory history;
    history.addCommand(std::make_unique<DummyCommand>(1));
    history.addCommand(std::make_unique<DummyCommand>(2));

    QVERIFY(history.undo());
    QCOMPARE(history.count(), 1);
    QCOMPARE(history.canRedo(), true);

    QVERIFY(history.undo());
    QCOMPARE(history.count(), 0);
    QCOMPARE(history.canUndo(), false);
    QCOMPARE(history.canRedo(), true);

    QVERIFY(history.redo());
    QCOMPARE(history.count(), 1);
    QCOMPARE(history.canUndo(), true);
}

void TestCommandHistory::testClear() {
    CommandHistory history;
    history.addCommand(std::make_unique<DummyCommand>(1));
    history.addCommand(std::make_unique<DummyCommand>(2));
    history.undo();

    history.clear();
    QCOMPARE(history.count(), 0);
    QCOMPARE(history.canUndo(), false);
    QCOMPARE(history.canRedo(), false);
}

void TestCommandHistory::testRedoInvalidationOnNewCommand() {
    CommandHistory history;
    history.addCommand(std::make_unique<DummyCommand>(1));
    history.addCommand(std::make_unique<DummyCommand>(2));
    history.undo();
    QCOMPARE(history.canRedo(), true);

    // Adding a new command clears the redo stack
    history.addCommand(std::make_unique<DummyCommand>(3));
    QCOMPARE(history.canRedo(), false);
    QCOMPARE(history.count(), 2);
}

QTEST_MAIN(TestCommandHistory)
#include "test_command_history.moc"
