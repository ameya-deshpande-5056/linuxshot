#include "Application.h"
#include "export/ImageExporter.h"
#include "ui/AboutDialog.h"
#include <QGuiApplication>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QClipboard>

Application::Application(QObject *parent)
    : QObject(parent)
{
}

Application::~Application() {
    if (m_lockFile) {
        m_lockFile->unlock();
    }
}

bool Application::init() {
    // Single instance enforcement
    QString runtimeDir = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    if (runtimeDir.isEmpty()) {
        runtimeDir = QDir::tempPath();
    }
    QString lockPath = QDir(runtimeDir).filePath("linuxshot.lock");
    m_lockFile = std::make_unique<QLockFile>(lockPath);
    if (!m_lockFile->tryLock(100)) {
        qWarning() << "Another instance of LinuxShot is already running.";
        return false;
    }

    m_settings = std::make_unique<SettingsManager>(this);
    m_shortcuts = std::make_unique<GlobalShortcutManager>(this);
    m_capture = std::make_unique<CaptureManager>(this);
    m_export = std::make_unique<ExportManager>(this);
    m_upload = std::make_unique<UploadManager>(this);
    m_tray = std::make_unique<TrayIcon>(this);
    m_overlay = std::make_unique<SelectionOverlay>();

    m_upload->setEndpoint(m_settings->uploadEndpoint());
    m_upload->setPrntscrEnabled(m_settings->uploadPrntscrEnabled());

    // Connect tray
    connect(m_tray.get(), &TrayIcon::captureRequested, this, &Application::triggerCapture);
    connect(m_tray.get(), &TrayIcon::preferencesRequested, this, &Application::showPreferences);
    connect(m_tray.get(), &TrayIcon::aboutRequested, this, &Application::showAbout);
    connect(m_tray.get(), &TrayIcon::quitRequested, qApp, &QCoreApplication::quit);

    // Connect global shortcuts
    connect(m_shortcuts.get(), &GlobalShortcutManager::shortcutActivated, this, &Application::onShortcutActivated);
    connect(m_shortcuts.get(), &GlobalShortcutManager::registrationFailed, this, [](const QString &id, const QString &err) {
        qWarning() << "Failed to register global shortcut" << id << ":" << err;
    });

    // Connect overlay actions
    connect(m_overlay.get(), &SelectionOverlay::saveRequested, this, &Application::onOverlaySave);
    connect(m_overlay.get(), &SelectionOverlay::copyRequested, this, &Application::onOverlayCopy);
    connect(m_overlay.get(), &SelectionOverlay::uploadRequested, this, &Application::onOverlayUpload);

    // Connect export manager signals
    connect(m_export.get(), &ExportManager::imageSaved, this, [this](const QString &path) {
        if (m_settings->showNotifications() && m_tray->isVisible()) {
            m_tray->showMessage("Screenshot Saved", QString("Saved to %1").arg(path), QSystemTrayIcon::Information, 3000);
        }
    });

    connect(m_export.get(), &ExportManager::imageCopied, this, [this]() {
        if (m_settings->showNotifications() && m_tray->isVisible()) {
            m_tray->showMessage("Screenshot Copied", "Image copied to clipboard", QSystemTrayIcon::Information, 2500);
        }
    });

    connect(m_export.get(), &ExportManager::saveFailed, this, [this](const QString &err) {
        if (m_tray->isVisible()) {
            m_tray->showMessage("Error Saving Screenshot", err, QSystemTrayIcon::Warning, 4000);
        }
    });

    // Connect upload manager signals
    connect(m_upload.get(), &UploadManager::uploadCompleted, this, &Application::onUploadFinished);

    // Settings changes
    connect(m_settings.get(), &SettingsManager::settingsChanged, this, &Application::reloadHotkeys);
    connect(m_settings.get(), &SettingsManager::settingsChanged, this, [this]() {
        m_upload->setEndpoint(m_settings->uploadEndpoint());
    m_upload->setPrntscrEnabled(m_settings->uploadPrntscrEnabled());
    });

    reloadHotkeys();

    m_tray->show();
    return true;
}

void Application::reloadHotkeys() {
    m_shortcuts->unregisterAll();

    if (m_settings->generalHotkeyEnabled()) {
        QString key = m_settings->generalHotkey();
        if (!key.isEmpty()) {
            m_shortcuts->registerShortcut("general", QKeySequence(key));
            m_tray->updateTooltip(key);
        }
    }

    if (m_settings->saveFullscreenHotkeyEnabled()) {
        QString key = m_settings->saveFullscreenHotkey();
        if (!key.isEmpty()) {
            m_shortcuts->registerShortcut("save_fullscreen", QKeySequence(key));
        }
    }

    if (m_settings->uploadFullscreenHotkeyEnabled()) {
        QString key = m_settings->uploadFullscreenHotkey();
        if (!key.isEmpty()) {
            m_shortcuts->registerShortcut("upload_fullscreen", QKeySequence(key));
        }
    }
}

void Application::onShortcutActivated(const QString &id) {
    if (id == "general") {
        triggerCapture();
    } else if (id == "save_fullscreen") {
        triggerFullscreenSave();
    } else if (id == "upload_fullscreen") {
        triggerFullscreenUpload();
    }
}

void Application::triggerCapture() {
    QImage desktop = m_capture->captureDesktop();
    if (!desktop.isNull()) {
        onCaptureCompleted(desktop);
    }
}

void Application::triggerFullscreenSave() {
    QImage desktop = m_capture->captureDesktop();
    if (desktop.isNull()) return;

    QString dir = m_settings->defaultSaveDir();
    QString fmt = m_settings->defaultFormat();
    int quality = m_settings->jpegQuality();

    QString filename = ImageExporter::generateDefaultFilename(fmt);
    QString filePath = QDir(dir).filePath(filename);

    m_export->saveToFile(desktop, filePath, fmt, quality);
}

void Application::triggerFullscreenUpload() {
    QImage desktop = m_capture->captureDesktop();
    if (desktop.isNull()) return;

    UploadOptions opts;
    opts.format = m_settings->defaultFormat();
    opts.jpegQuality = m_settings->jpegQuality();
    opts.customEndpoint = m_settings->uploadEndpoint();

    m_upload->upload(desktop, opts);
}

void Application::onCaptureCompleted(const QImage &screenshot) {
    m_overlay->setScreenshot(screenshot);

    if (m_settings->keepSelection()) {
        QRect prev = m_settings->lastSelection();
        if (!prev.isEmpty()) {
            m_overlay->setSelectionRect(prev);
        }
    }

    m_overlay->showFullScreen();
}

void Application::onOverlaySave(const QImage &image) {
    if (m_settings->keepSelection()) {
        m_settings->setLastSelection(m_overlay->selectionRect());
    }

    m_overlay->hide();
    QGuiApplication::processEvents();

    m_export->saveWithDialog(
        image,
        nullptr,
        m_settings->defaultSaveDir(),
        m_settings->defaultFormat(),
        m_settings->jpegQuality()
    );
}

void Application::onOverlayCopy(const QImage &image) {
    if (m_settings->keepSelection()) {
        m_settings->setLastSelection(m_overlay->selectionRect());
    }

    m_export->copyToClipboard(image);
}

void Application::onOverlayUpload(const QImage &image) {
    if (m_settings->keepSelection()) {
        m_settings->setLastSelection(m_overlay->selectionRect());
    }

    UploadOptions opts;
    opts.format = m_settings->defaultFormat();
    opts.jpegQuality = m_settings->jpegQuality();
    opts.customEndpoint = m_settings->uploadEndpoint();

    m_upload->upload(image, opts);
}

void Application::onUploadFinished(const UploadResult &result) {
    if (result.success) {
        if (m_settings->autoCopyLink()) {
            QClipboard *clipboard = QGuiApplication::clipboard();
            if (clipboard) {
                clipboard->setText(result.imageUrl.toString());
            }
        }

        if (m_settings->showNotifications() && m_tray->isVisible()) {
            m_tray->showMessage(
                "Screenshot Uploaded",
                QString("Link copied to clipboard: %1").arg(result.imageUrl.toString()),
                QSystemTrayIcon::Information,
                5000
            );
        }
    } else {
        if (m_tray->isVisible()) {
            m_tray->showMessage(
                "Upload Failed",
                result.errorMessage,
                QSystemTrayIcon::Warning,
                5000
            );
        }
    }
}

void Application::showPreferences() {
    SettingsDialog dlg(m_settings.get());
    dlg.exec();
}

void Application::showAbout() {
    AboutDialog dlg;
    dlg.exec();
}
