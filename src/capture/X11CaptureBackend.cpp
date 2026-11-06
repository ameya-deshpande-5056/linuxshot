#include "X11CaptureBackend.h"
#include <QGuiApplication>
#include <QScreen>
#include <QPainter>
#include <QDebug>

X11CaptureBackend::X11CaptureBackend() {
    if (QGuiApplication::platformName().compare("xcb", Qt::CaseInsensitive) == 0) {
        m_connection = xcb_connect(nullptr, nullptr);
        if (!xcb_connection_has_error(m_connection)) {
            const xcb_setup_t *setup = xcb_get_setup(m_connection);
            xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
            if (iter.rem > 0) {
                m_rootWindow = iter.data->root;
            }
            initShm();
        }
    }
}

X11CaptureBackend::~X11CaptureBackend() {
    cleanupShm();
    if (m_connection) {
        xcb_disconnect(m_connection);
        m_connection = nullptr;
    }
}

bool X11CaptureBackend::isAvailable() const {
    return (m_connection != nullptr && !xcb_connection_has_error(m_connection));
}

bool X11CaptureBackend::initShm() {
    if (!m_connection) return false;

    xcb_shm_query_version_cookie_t cookie = xcb_shm_query_version(m_connection);
    xcb_shm_query_version_reply_t *reply = xcb_shm_query_version_reply(m_connection, cookie, nullptr);
    if (!reply) return false;

    bool hasShm = (reply->shared_pixmaps && reply->major_version >= 1);
    free(reply);
    m_shmAvailable = hasShm;
    return m_shmAvailable;
}

void X11CaptureBackend::cleanupShm() {
    if (m_shmAddr && m_shmAddr != reinterpret_cast<uint8_t *>(-1)) {
        shmdt(m_shmAddr);
        m_shmAddr = nullptr;
    }
    if (m_shmId != -1) {
        shmctl(m_shmId, IPC_RMID, nullptr);
        m_shmId = -1;
    }
    if (m_connection && m_shmSeg) {
        xcb_shm_detach(m_connection, m_shmSeg);
        m_shmSeg = 0;
    }
    m_shmSize = 0;
}

QList<QRect> X11CaptureBackend::screenGeometries() {
    QList<QRect> geoms;
    const auto screens = QGuiApplication::screens();
    for (QScreen *screen : screens) {
        geoms.append(screen->geometry());
    }
    return geoms;
}

QImage X11CaptureBackend::captureScreen(int screenIndex) {
    const auto screens = QGuiApplication::screens();
    if (screens.isEmpty()) return QImage();

    QScreen *target = nullptr;
    if (screenIndex >= 0 && screenIndex < screens.size()) {
        target = screens.at(screenIndex);
    } else {
        target = QGuiApplication::primaryScreen();
    }

    if (!target) return QImage();

    const qreal dpr = target->devicePixelRatio();
    const QRect geom = target->geometry();
    const QRect physGeom(
        qRound(geom.x() * dpr),
        qRound(geom.y() * dpr),
        qRound(geom.width() * dpr),
        qRound(geom.height() * dpr)
    );

    // Prefer fast SHM capture if available
    if (m_shmAvailable && isAvailable()) {
        QImage shmImg = captureViaShm(physGeom);
        if (!shmImg.isNull()) {
            return shmImg;
        }
    }

    // High-fidelity fallback using Qt's screen grab (which handles HiDPI & composition)
    QPixmap pm = target->grabWindow(0);
    QImage img = pm.toImage();
    img.setDevicePixelRatio(1.0);
    return img;
}

QImage X11CaptureBackend::captureFullDesktop() {
    const auto screens = QGuiApplication::screens();
    if (screens.isEmpty()) return QImage();

    // Query native physical root window geometry if available
    QRect rootGeom;
    if (m_connection && m_rootWindow != 0) {
        xcb_get_geometry_cookie_t cookie = xcb_get_geometry(m_connection, m_rootWindow);
        xcb_get_geometry_reply_t *reply = xcb_get_geometry_reply(m_connection, cookie, nullptr);
        if (reply) {
            rootGeom = QRect(reply->x, reply->y, reply->width, reply->height);
            free(reply);
        }
    }

    // Fallback physical bounding calculation across screens
    if (rootGeom.isEmpty()) {
        for (QScreen *screen : screens) {
            const qreal dpr = screen->devicePixelRatio();
            const QRect g = screen->geometry();
            const QRect phys(
                qRound(g.x() * dpr),
                qRound(g.y() * dpr),
                qRound(g.width() * dpr),
                qRound(g.height() * dpr)
            );
            rootGeom = rootGeom.united(phys);
        }
    }

    if (m_shmAvailable && isAvailable() && !rootGeom.isEmpty()) {
        QImage shmImg = captureViaShm(rootGeom);
        if (!shmImg.isNull()) {
            return shmImg;
        }
    }

    // Stitch together screens with native grabWindow
    QRect physBounding;
    for (QScreen *screen : screens) {
        const qreal dpr = screen->devicePixelRatio();
        const QRect g = screen->geometry();
        const QRect phys(
            qRound(g.x() * dpr),
            qRound(g.y() * dpr),
            qRound(g.width() * dpr),
            qRound(g.height() * dpr)
        );
        physBounding = physBounding.united(phys);
    }

    if (physBounding.isEmpty()) return QImage();

    QImage combined(physBounding.size(), QImage::Format_ARGB32_Premultiplied);
    combined.fill(Qt::black);

    QPainter painter(&combined);
    for (QScreen *screen : screens) {
        const qreal dpr = screen->devicePixelRatio();
        const QRect g = screen->geometry();
        const QRect phys(
            qRound(g.x() * dpr),
            qRound(g.y() * dpr),
            qRound(g.width() * dpr),
            qRound(g.height() * dpr)
        );
        QPixmap pm = screen->grabWindow(0);
        if (!pm.isNull()) {
            QImage screenImg = pm.toImage();
            screenImg.setDevicePixelRatio(1.0);
            painter.drawImage(phys.topLeft() - physBounding.topLeft(), screenImg);
        }
    }
    painter.end();

    return combined;
}

QImage X11CaptureBackend::captureViaShm(const QRect &geometry) {
    if (!m_connection || geometry.isEmpty()) return QImage();

    const size_t bytesPerLine = geometry.width() * 4;
    const size_t totalBytes = bytesPerLine * geometry.height();

    // Reallocate SHM segment if needed
    if (m_shmSize < totalBytes || !m_shmAddr) {
        cleanupShm();

        m_shmId = shmget(IPC_PRIVATE, totalBytes, IPC_CREAT | 0777);
        if (m_shmId == -1) return QImage();

        m_shmAddr = static_cast<uint8_t *>(shmat(m_shmId, nullptr, 0));
        if (m_shmAddr == reinterpret_cast<uint8_t *>(-1)) {
            m_shmAddr = nullptr;
            return QImage();
        }

        m_shmSeg = xcb_generate_id(m_connection);
        xcb_shm_attach(m_connection, m_shmSeg, m_shmId, 0);
        m_shmSize = totalBytes;
    }

    xcb_shm_get_image_cookie_t cookie = xcb_shm_get_image(
        m_connection, m_rootWindow,
        geometry.x(), geometry.y(),
        geometry.width(), geometry.height(),
        ~0, XCB_IMAGE_FORMAT_Z_PIXMAP,
        m_shmSeg, 0
    );

    xcb_shm_get_image_reply_t *reply = xcb_shm_get_image_reply(m_connection, cookie, nullptr);
    if (!reply) return QImage();
    free(reply);

    // XCB ZPixmap 32-bit format is BGRA/BGRX; construct QImage and deep copy to detach
    QImage img(m_shmAddr, geometry.width(), geometry.height(), bytesPerLine, QImage::Format_RGB32);
    return img.copy();
}

