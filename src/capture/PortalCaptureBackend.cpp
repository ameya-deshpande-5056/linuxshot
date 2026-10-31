#include "PortalCaptureBackend.h"
#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QTime>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMessage>
#include <QUrl>
#include <QFile>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

namespace {
class PortalResponseHandler : public QObject {
    Q_OBJECT
public:
    QString uri;
    bool received = false;

public slots:
    void handleResponse(uint code, const QVariantMap &results) {
        if (code == 0 && results.contains("uri")) {
            uri = results.value("uri").toString();
        }
        received = true;
        emit finished();
    }

signals:
    void finished();
};
}

PortalCaptureBackend::PortalCaptureBackend(QObject *parent)
    : QObject(parent)
{
    QDBusInterface portal(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.DBus.Properties",
        QDBusConnection::sessionBus()
    );
    m_portalAvailable = portal.isValid();
}

bool PortalCaptureBackend::isAvailable() const {
    return m_portalAvailable;
}

QList<QRect> PortalCaptureBackend::screenGeometries() {
    QList<QRect> geoms;
    const auto screens = QGuiApplication::screens();
    for (QScreen *s : screens) {
        geoms.append(s->geometry());
    }
    return geoms;
}

QImage PortalCaptureBackend::captureScreen(int screenIndex) {
    Q_UNUSED(screenIndex);
    return captureFullDesktop();
}

QImage PortalCaptureBackend::captureFullDesktop() {
    if (!m_portalAvailable) {
        QScreen *primary = QGuiApplication::primaryScreen();
        return primary ? primary->grabWindow(0).toImage() : QImage();
    }

    QDBusInterface screenshotInterface(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.Screenshot",
        QDBusConnection::sessionBus()
    );

    if (!screenshotInterface.isValid()) {
        QScreen *primary = QGuiApplication::primaryScreen();
        return primary ? primary->grabWindow(0).toImage() : QImage();
    }

    QVariantMap options;
    options["interactive"] = false;
    options["handle_token"] = QString("linuxshot_%1").arg(QTime::currentTime().msec());

    QDBusMessage reply = screenshotInterface.call("Screenshot", "", options);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Portal Screenshot call error:" << reply.errorMessage();
        QScreen *primary = QGuiApplication::primaryScreen();
        return primary ? primary->grabWindow(0).toImage() : QImage();
    }

    if (reply.arguments().isEmpty()) {
        return QImage();
    }

    QDBusObjectPath requestPath = reply.arguments().at(0).value<QDBusObjectPath>();

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(10000);

    PortalResponseHandler handler;
    QDBusConnection::sessionBus().connect(
        "org.freedesktop.portal.Desktop",
        requestPath.path(),
        "org.freedesktop.portal.Request",
        "Response",
        &handler,
        SLOT(handleResponse(uint,QVariantMap))
    );

    connect(&handler, &PortalResponseHandler::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec();

    if (!handler.uri.isEmpty()) {
        QUrl url(handler.uri);
        QString localPath = url.toLocalFile();
        if (QFile::exists(localPath)) {
            return QImage(localPath);
        }
    }

    QScreen *primary = QGuiApplication::primaryScreen();
    return primary ? primary->grabWindow(0).toImage() : QImage();
}

#include "PortalCaptureBackend.moc"
