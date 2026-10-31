#pragma once

#include <QObject>
#include <memory>
#include <QLockFile>
#include "SettingsManager.h"
#include "GlobalShortcutManager.h"
#include "capture/CaptureManager.h"
#include "export/ExportManager.h"
#include "upload/UploadManager.h"
#include "ui/TrayIcon.h"
#include "ui/SelectionOverlay.h"
#include "ui/SettingsDialog.h"

class Application : public QObject {
    Q_OBJECT

public:
    explicit Application(QObject *parent = nullptr);
    ~Application() override;

    bool init();

public slots:
    void triggerCapture();
    void triggerFullscreenSave();
    void triggerFullscreenUpload();
    void showPreferences();
    void showAbout();

private slots:
    void onShortcutActivated(const QString &id);
    void onCaptureCompleted(const QImage &screenshot);
    void onOverlaySave(const QImage &image);
    void onOverlayCopy(const QImage &image);
    void onOverlayUpload(const QImage &image);
    void onUploadFinished(const UploadResult &result);
    void reloadHotkeys();

private:
    std::unique_ptr<SettingsManager> m_settings;
    std::unique_ptr<GlobalShortcutManager> m_shortcuts;
    std::unique_ptr<CaptureManager> m_capture;
    std::unique_ptr<ExportManager> m_export;
    std::unique_ptr<UploadManager> m_upload;
    std::unique_ptr<TrayIcon> m_tray;
    std::unique_ptr<SelectionOverlay> m_overlay;

    std::unique_ptr<QLockFile> m_lockFile;
};
