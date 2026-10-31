#include "Platform.h"
#include <QGuiApplication>
#include <QProcessEnvironment>
#include <QDBusInterface>
#include <QDBusConnection>

Platform::WindowingSystem Platform::windowingSystem() {
    const QString platform = QGuiApplication::platformName().toLower();
    if (platform.contains("wayland")) {
        return WindowingSystem::Wayland;
    }
    if (platform.contains("xcb")) {
        return WindowingSystem::X11;
    }

    // Check environment variable fallback
    const QString xdgType = qEnvironmentVariable("XDG_SESSION_TYPE").toLower();
    if (xdgType == "wayland") return WindowingSystem::Wayland;
    if (xdgType == "x11") return WindowingSystem::X11;

    return WindowingSystem::Unknown;
}

bool Platform::isX11() {
    return windowingSystem() == WindowingSystem::X11;
}

bool Platform::isWayland() {
    return windowingSystem() == WindowingSystem::Wayland;
}

Platform::DesktopEnvironment Platform::desktopEnvironment() {
    QString desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP").toLower();
    if (desktop.contains("cinnamon")) return DesktopEnvironment::Cinnamon;
    if (desktop.contains("gnome"))    return DesktopEnvironment::GNOME;
    if (desktop.contains("kde"))      return DesktopEnvironment::KDE;
    if (desktop.contains("xfce"))     return DesktopEnvironment::XFCE;
    if (desktop.contains("mate"))     return DesktopEnvironment::MATE;
    return DesktopEnvironment::Other;
}

QString Platform::desktopEnvironmentName() {
    switch (desktopEnvironment()) {
        case DesktopEnvironment::Cinnamon: return "Cinnamon";
        case DesktopEnvironment::GNOME:    return "GNOME";
        case DesktopEnvironment::KDE:      return "KDE Plasma";
        case DesktopEnvironment::XFCE:     return "XFCE";
        case DesktopEnvironment::MATE:     return "MATE";
        default:
            QString raw = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
            return raw.isEmpty() ? "Unknown" : raw;
    }
}

QString Platform::sessionType() {
    QString st = qEnvironmentVariable("XDG_SESSION_TYPE");
    if (st.isEmpty()) {
        return isX11() ? "x11" : (isWayland() ? "wayland" : "unknown");
    }
    return st.toLower();
}

bool Platform::isPortalAvailable() {
    QDBusInterface portal(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.DBus.Properties",
        QDBusConnection::sessionBus()
    );
    return portal.isValid();
}
