#include <QTest>
#include "annotation/PenTool.h"
#include "annotation/LineTool.h"
#include "annotation/ArrowTool.h"
#include "annotation/RectTool.h"
#include "annotation/CircleTool.h"
#include "annotation/MarkerTool.h"
#include "annotation/TextTool.h"
#include "annotation/BlurTool.h"
#include "annotation/CommandHistory.h"
#include "ui/FloatingTextEditor.h"

class TestAnnotationTools : public QObject {
    Q_OBJECT

private slots:
    void testPenTool();
    void testLineTool();
    void testArrowTool();
    void testRectTool();
    void testCircleTool();
    void testMarkerTool();
    void testTextTool();
    void testMultiLineTextTool();
    void testBlurTool();
    void testFloatingTextEditorTopMargin();
};

void TestAnnotationTools::testPenTool() {
    PenTool tool;
    tool.setColor(Qt::red);
    tool.setWidth(4);

    tool.mousePress(QPointF(10, 10));
    QVERIFY(tool.isDrawing());
    tool.mouseMove(QPointF(20, 20));
    tool.mouseMove(QPointF(30, 25));
    tool.mouseRelease(QPointF(40, 30));
    QVERIFY(!tool.isDrawing());

    auto cmd = tool.createCommand();
    QVERIFY(cmd != nullptr);
    QCOMPARE(cmd->description(), "Pen Stroke");

    QImage img(100, 100, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    QPainter p(&img);
    cmd->paint(p);
    p.end();

    // Check pixel at (10, 10) or along path changed
    QVERIFY(img.pixelColor(20, 20) != Qt::white);
}

void TestAnnotationTools::testLineTool() {
    LineTool tool;
    tool.setColor(Qt::blue);
    tool.setWidth(2);

    tool.mousePress(QPointF(0, 0));
    tool.mouseMove(QPointF(50, 50));
    tool.mouseRelease(QPointF(100, 100));

    auto cmd = tool.createCommand();
    QVERIFY(cmd != nullptr);
    QCOMPARE(cmd->description(), "Line");

    QImage img(120, 120, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    QPainter p(&img);
    cmd->paint(p);
    p.end();

    QVERIFY(img.pixelColor(50, 50) != Qt::white);
}

void TestAnnotationTools::testArrowTool() {
    ArrowTool tool;
    tool.setColor(Qt::green);
    tool.setWidth(3);

    tool.mousePress(QPointF(10, 50));
    tool.mouseRelease(QPointF(90, 50));

    auto cmd = tool.createCommand();
    QVERIFY(cmd != nullptr);
    QCOMPARE(cmd->description(), "Arrow");

    QImage img(100, 100, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    QPainter p(&img);
    cmd->paint(p);
    p.end();

    QVERIFY(img.pixelColor(50, 50) != Qt::white);
}

void TestAnnotationTools::testRectTool() {
    RectTool tool;
    tool.setColor(Qt::red);
    tool.setWidth(2);

    tool.mousePress(QPointF(10, 10));
    tool.mouseRelease(QPointF(80, 80));

    auto cmd = tool.createCommand();
    QVERIFY(cmd != nullptr);
    QCOMPARE(cmd->description(), "Rectangle");

    QImage img(100, 100, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    QPainter p(&img);
    cmd->paint(p);
    p.end();

    QVERIFY(img.pixelColor(10, 50) != Qt::white);
}

void TestAnnotationTools::testCircleTool() {
    CircleTool tool;
    tool.setColor(Qt::magenta);
    tool.setWidth(2);

    tool.mousePress(QPointF(20, 20));
    tool.mouseRelease(QPointF(80, 80));

    auto cmd = tool.createCommand();
    QVERIFY(cmd != nullptr);
    QCOMPARE(cmd->description(), "Circle");

    QImage img(100, 100, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    QPainter p(&img);
    cmd->paint(p);
    p.end();

    // Center of circle outline is empty
    QCOMPARE(img.pixelColor(50, 50), QColor(Qt::white));
    // Perimeter has color
    QVERIFY(img.pixelColor(20, 50) != Qt::white || img.pixelColor(50, 20) != Qt::white);
}

void TestAnnotationTools::testMarkerTool() {
    MarkerTool tool;
    tool.mousePress(QPointF(10, 50));
    tool.mouseRelease(QPointF(90, 50));

    auto cmd = tool.createCommand();
    QVERIFY(cmd != nullptr);
    QCOMPARE(cmd->description(), "Highlighter Marker");

    QImage img(100, 100, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    QPainter p(&img);
    cmd->paint(p);
    p.end();

    QColor c = img.pixelColor(50, 50);
    QVERIFY(c != Qt::white);
}

void TestAnnotationTools::testTextTool() {
    TextTool tool;
    tool.mousePress(QPointF(20, 40));
    tool.setText("LinuxShot");

    auto cmd = tool.createCommand();
    QVERIFY(cmd != nullptr);
    QCOMPARE(cmd->description(), "Text");

    QImage img(150, 80, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    QPainter p(&img);
    cmd->paint(p);
    p.end();

    // Some pixels in the text region must be colored
    bool foundNonWhite = false;
    for (int y = 20; y < 60; ++y) {
        for (int x = 20; x < 120; ++x) {
            if (img.pixelColor(x, y) != Qt::white) {
                foundNonWhite = true;
                break;
            }
        }
        if (foundNonWhite) break;
    }
    QVERIFY(foundNonWhite);
}

void TestAnnotationTools::testMultiLineTextTool() {
    TextCommand cmd("Line 1\nLine 2", QPointF(20, 30), QFont("Sans Serif", 12), Qt::red);
    QCOMPARE(cmd.text(), "Line 1\nLine 2");
    QCOMPARE(cmd.pos(), QPointF(20, 30));

    QImage img(200, 120, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    QPainter p(&img);
    cmd.paint(p);
    p.end();

    // Line 1 should be around y=30, line 2 should be around y=30 + lineSpacing
    bool foundLine1 = false;
    for (int y = 15; y < 40; ++y) {
        for (int x = 20; x < 100; ++x) {
            if (img.pixelColor(x, y) != Qt::white) {
                foundLine1 = true;
                break;
            }
        }
        if (foundLine1) break;
    }
    QVERIFY(foundLine1);

    bool foundLine2 = false;
    for (int y = 40; y < 70; ++y) {
        for (int x = 20; x < 100; ++x) {
            if (img.pixelColor(x, y) != Qt::white) {
                foundLine2 = true;
                break;
            }
        }
        if (foundLine2) break;
    }
    QVERIFY(foundLine2);
}

void TestAnnotationTools::testBlurTool() {
    BlurTool tool;
    tool.mousePress(QPointF(10, 10));
    tool.mouseRelease(QPointF(60, 60));

    auto cmd = tool.createCommand();
    QVERIFY(cmd != nullptr);
    QCOMPARE(cmd->description(), "Blur / Pixelate");

    // Checkerboard image
    QImage img(100, 100, QImage::Format_RGB32);
    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 100; ++x) {
            img.setPixelColor(x, y, ((x + y) % 2 == 0) ? Qt::black : Qt::white);
        }
    }

    cmd->applyToImage(img);

    // Pixels inside the blurred block should now be averaged (gray) instead of strictly black/white
    QColor c1 = img.pixelColor(20, 20);
    QColor c2 = img.pixelColor(21, 20);
    QCOMPARE(c1, c2); // In a pixelated mosaic block, adjacent pixels have identical color!
}

void TestAnnotationTools::testFloatingTextEditorTopMargin() {
    QWidget parent;
    parent.resize(800, 600);

    FloatingTextEditor editor(&parent);
    QFont font("Sans Serif", 14, QFont::Bold);
    QFontMetrics fm(font);

    // Position directly on the top margin (y = 0)
    QRect topRect(50, 0, 200, 38);
    editor.startEditing(topRect, font, Qt::red, "Top Margin Text");

    QCOMPARE(editor.text(), "Top Margin Text");
    QCOMPARE(editor.y(), 0);

    // Baseline calculation must have no header offset:
    // y = editor.y() + BorderMargin + DocMargin + fm.ascent()
    // With BorderMargin = 5, DocMargin = 2, it is 0 + 7 + fm.ascent()
    QPointF baseline = editor.baselinePos();
    QCOMPARE(baseline.y(), 0 + 5 + 2 + fm.ascent());

    // Verify commit signal
    QString committedText;
    QPointF committedPos;
    QObject::connect(&editor, &FloatingTextEditor::committed,
                     [&](const QString &t, const QPointF &p, const QFont &, const QColor &) {
        committedText = t;
        committedPos = p;
    });

    editor.commit();
    QCOMPARE(committedText, "Top Margin Text");
    QCOMPARE(committedPos, baseline);
    QVERIFY(!editor.isEditing());
}

QTEST_MAIN(TestAnnotationTools)
#include "test_annotation_tools.moc"

