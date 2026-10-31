#include "TextTool.h"
#include <QPainterPath>
#include <QTextLayout>
#include <QTextOption>

TextCommand::TextCommand(const QString &text, const QPointF &pos, const QFont &font, const QColor &color, int boxWidth)
    : m_text(text), m_pos(pos), m_font(font), m_color(color), m_boxWidth(boxWidth)
{
}

QString TextCommand::wrapText(const QString &text, const QFont &font, qreal wrapWidth) {
    if (wrapWidth <= 0 || text.isEmpty()) return text;

    QStringList paragraphs = text.split('\n');
    QStringList resultLines;

    for (const QString &para : paragraphs) {
        if (para.isEmpty()) {
            resultLines.append(QString());
            continue;
        }

        QTextLayout layout(para, font);
        QTextOption opt;
        opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        layout.setTextOption(opt);
        layout.beginLayout();

        while (true) {
            QTextLine line = layout.createLine();
            if (!line.isValid()) break;
            line.setLineWidth(wrapWidth);
            QString lineStr = para.mid(line.textStart(), line.textLength());
            while (lineStr.endsWith(' ') || lineStr.endsWith('\t') || lineStr.endsWith('\r')) {
                lineStr.chop(1);
            }
            resultLines.append(lineStr);
        }
        layout.endLayout();
    }

    return resultLines.join('\n');
}

void TextCommand::paint(QPainter &painter) const {
    if (m_text.isEmpty()) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setFont(m_font);
    painter.setPen(m_color);

    QFontMetrics fm(m_font);
    int lineSpacing = fm.lineSpacing();
    QString textToDraw = (m_boxWidth > 0) ? wrapText(m_text, m_font, m_boxWidth) : m_text;
    QStringList lines = textToDraw.split('\n');

    QPen outlinePen(QColor(0, 0, 0, 160), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].isEmpty()) continue;
        QPointF linePos = m_pos + QPointF(0, i * lineSpacing);
        QPainterPath path;
        path.addText(linePos, m_font, lines[i]);
        painter.strokePath(path, outlinePen);
        painter.fillPath(path, m_color);
    }

    painter.restore();
}

std::unique_ptr<AnnotationCommand> TextCommand::clone() const {
    return std::make_unique<TextCommand>(m_text, m_pos, m_font, m_color, m_boxWidth);
}

void TextTool::mousePress(const QPointF &pos) {
    m_clickPos = pos;
    m_active = true;
}

void TextTool::mouseMove(const QPointF &pos) {
    Q_UNUSED(pos);
}

void TextTool::mouseRelease(const QPointF &pos) {
    Q_UNUSED(pos);
}

void TextTool::paintPreview(QPainter &painter) const {
    if (!m_active || m_text.isEmpty()) return;
    TextCommand cmd(m_text, m_clickPos, m_font, m_color);
    cmd.paint(painter);
}

std::unique_ptr<AnnotationCommand> TextTool::createCommand() {
    if (m_text.trimmed().isEmpty()) {
        m_active = false;
        return nullptr;
    }
    auto cmd = std::make_unique<TextCommand>(m_text, m_clickPos, m_font, m_color);
    m_text.clear();
    m_active = false;
    return cmd;
}

void TextTool::reset() {
    m_text.clear();
    m_active = false;
}
