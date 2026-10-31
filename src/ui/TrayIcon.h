#pragma once

#include <QSystemTrayIcon>
#include <QMenu>

class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT

public:
    explicit TrayIcon(QObject *parent = nullptr);
    ~TrayIcon() override = default;

    void updateTooltip(const QString &hotkey);

signals:
    void captureRequested();
    void preferencesRequested();
    void aboutRequested();
    void quitRequested();

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createMenu();
    QMenu *m_menu = nullptr;
};
