#include "BlurTool.h"

BlurCommand::BlurCommand(const QRectF &rect, int blockSize)
    : m_rect(rect.normalized()), m_blockSize(qMax(4, blockSize))
{
}

void BlurCommand::cachePixelation(const QImage &sourceImage) {
    if (sourceImage.isNull() || m_rect.isEmpty()) return;

    QRect target = m_rect.toRect().intersected(sourceImage.rect());
    if (target.isEmpty()) return;

    QImage cropped = sourceImage.copy(target);
    // Downscale and upscale to create crisp pixelated mosaic
    int smallW = qMax(1, cropped.width() / m_blockSize);
    int smallH = qMax(1, cropped.height() / m_blockSize);

    QImage mosaic = cropped.scaled(smallW, smallH, Qt::IgnoreAspectRatio, Qt::FastTransformation)
                           .scaled(cropped.size(), Qt::IgnoreAspectRatio, Qt::FastTransformation);

    m_cachedMosaic = QPixmap::fromImage(mosaic);
}

void BlurCommand::paint(QPainter &painter) const {
    if (!m_cachedMosaic.isNull()) {
        painter.drawPixmap(m_rect.toRect().topLeft(), m_cachedMosaic);
    } else {
        // Fallback representation in preview
        painter.save();
        painter.setPen(QPen(QColor(100, 100, 100, 180), 1, Qt::DashLine));
        painter.setBrush(QColor(150, 150, 150, 120));
        painter.drawRect(m_rect);
        painter.restore();
    }
}

void BlurCommand::applyToImage(QImage &image) const {
    if (image.isNull() || m_rect.isEmpty()) return;

    QRect target = m_rect.toRect().intersected(image.rect());
    if (target.isEmpty()) return;

    int bx = target.x();
    int by = target.y();
    int bw = target.width();
    int bh = target.height();

    for (int y = by; y < by + bh; y += m_blockSize) {
        for (int x = bx; x < bx + bw; x += m_blockSize) {
            int blockW = qMin(m_blockSize, bx + bw - x);
            int blockH = qMin(m_blockSize, by + bh - y);

            uint64_t rSum = 0, gSum = 0, bSum = 0;
            int count = 0;

            for (int py = y; py < y + blockH; ++py) {
                const QRgb *line = reinterpret_cast<const QRgb *>(image.constScanLine(py));
                for (int px = x; px < x + blockW; ++px) {
                    QRgb pixel = line[px];
                    rSum += qRed(pixel);
                    gSum += qGreen(pixel);
                    bSum += qBlue(pixel);
                    count++;
                }
            }

            if (count > 0) {
                QRgb avgColor = qRgb(rSum / count, gSum / count, bSum / count);
                for (int py = y; py < y + blockH; ++py) {
                    QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(py));
                    for (int px = x; px < x + blockW; ++px) {
                        line[px] = avgColor;
                    }
                }
            }
        }
    }
}

std::unique_ptr<AnnotationCommand> BlurCommand::clone() const {
    auto cmd = std::make_unique<BlurCommand>(m_rect, m_blockSize);
    cmd->m_cachedMosaic = m_cachedMosaic;
    return cmd;
}

void BlurTool::mousePress(const QPointF &pos) {
    m_start = pos;
    m_current = pos;
    m_drawing = true;
}

void BlurTool::mouseMove(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
}

void BlurTool::mouseRelease(const QPointF &pos) {
    if (!m_drawing) return;
    m_current = pos;
    m_drawing = false;
}

void BlurTool::paintPreview(QPainter &painter) const {
    if (!m_drawing) return;
    QRectF r(m_start, m_current);
    painter.save();
    painter.setPen(QPen(QColor(50, 150, 255), 1, Qt::DashLine));
    painter.setBrush(QColor(100, 180, 255, 60));
    painter.drawRect(r.normalized());
    painter.restore();
}

std::unique_ptr<AnnotationCommand> BlurTool::createCommand() {
    QRectF r(m_start, m_current);
    if (r.normalized().width() < 4 || r.normalized().height() < 4) {
        return nullptr;
    }
    return std::make_unique<BlurCommand>(r);
}

void BlurTool::reset() {
    m_drawing = false;
}
