#include "TrayIcon.h"
#include <QIcon>

TrayIcon::TrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
{
    setIcon(QIcon(":/icons/linuxshot.png"));
    updateTooltip("Print");
    createMenu();
    connect(this, &QSystemTrayIcon::activated, this, &TrayIcon::onActivated);
}

void TrayIcon::updateTooltip(const QString &hotkey) {
    if (hotkey.isEmpty()) {
        setToolTip("LinuxShot — Screenshot Tool");
    } else {
        setToolTip(QString("LinuxShot — Press %1 to take a screenshot").arg(hotkey));
    }
}

void TrayIcon::createMenu() {
    m_menu = new QMenu();

    auto *captureAction = m_menu->addAction(QIcon(":/icons/linuxshot.png"), "Take a Screenshot");
    connect(captureAction, &QAction::triggered, this, &TrayIcon::captureRequested);

    m_menu->addSeparator();

    auto *prefsAction = m_menu->addAction("Preferences...");
    connect(prefsAction, &QAction::triggered, this, &TrayIcon::preferencesRequested);

    auto *aboutAction = m_menu->addAction("About LinuxShot");
    connect(aboutAction, &QAction::triggered, this, &TrayIcon::aboutRequested);

    m_menu->addSeparator();

    auto *quitAction = m_menu->addAction("Quit");
    connect(quitAction, &QAction::triggered, this, &TrayIcon::quitRequested);

    setContextMenu(m_menu);
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) { // Left click
        emit captureRequested();
    }
}
