#pragma once

#include <QString>

class Platform {
public:
    enum class WindowingSystem {
        X11,
        Wayland,
        Unknown
    };

    enum class DesktopEnvironment {
        GNOME,
        KDE,
        Cinnamon,
        XFCE,
        MATE,
        Other
    };

    static WindowingSystem windowingSystem();
    static bool isX11();
    static bool isWayland();

    static DesktopEnvironment desktopEnvironment();
    static QString desktopEnvironmentName();

    static QString sessionType();
    static bool isPortalAvailable();
};
