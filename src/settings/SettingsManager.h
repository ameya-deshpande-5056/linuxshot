#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QColor>
#include <QFont>
#include <QRect>
#include <memory>

class SettingsManager : public QObject {
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);
    explicit SettingsManager(const QString &customConfigPath, QObject *parent = nullptr);
    ~SettingsManager() override = default;

    // Hotkeys
    QString generalHotkey() const;
    void setGeneralHotkey(const QString &key);
    bool generalHotkeyEnabled() const;
    void setGeneralHotkeyEnabled(bool enabled);

    QString saveFullscreenHotkey() const;
    void setSaveFullscreenHotkey(const QString &key);
    bool saveFullscreenHotkeyEnabled() const;
    void setSaveFullscreenHotkeyEnabled(bool enabled);

    QString uploadFullscreenHotkey() const;
    void setUploadFullscreenHotkey(const QString &key);
    bool uploadFullscreenHotkeyEnabled() const;
    void setUploadFullscreenHotkeyEnabled(bool enabled);

    // General preferences
    QString language() const;
    void setLanguage(const QString &lang);

    bool showNotifications() const;
    void setShowNotifications(bool show);

    bool captureCursor() const;
    void setCaptureCursor(bool capture);

    bool keepSelection() const;
    void setKeepSelection(bool keep);

    QRect lastSelection() const;
    void setLastSelection(const QRect &rect);

    // Formats & Save
    QString defaultSaveDir() const;
    void setDefaultSaveDir(const QString &dir);

    QString defaultFormat() const;
    void setDefaultFormat(const QString &fmt);

    int jpegQuality() const;
    void setJpegQuality(int quality);

    // Upload & Network
    QString uploadEndpoint() const;
    void setUploadEndpoint(const QString &url);

    bool uploadPrntscrEnabled() const;
    void setUploadPrntscrEnabled(bool enabled);

    bool autoCopyLink() const;
    void setAutoCopyLink(bool autoCopy);

    bool autoCloseUploadWindow() const;
    void setAutoCloseUploadWindow(bool autoClose);

    int proxyType() const; // 0=None, 1=System, 2=Manual
    void setProxyType(int type);

    QString proxyHost() const;
    void setProxyHost(const QString &host);

    int proxyPort() const;
    void setProxyPort(int port);

    // Annotation tool defaults
    QColor penColor() const;
    void setPenColor(const QColor &color);

    int penWidth() const;
    void setPenWidth(int width);

    int markerWidth() const;
    void setMarkerWidth(int width);

    QFont textFont() const;
    void setTextFont(const QFont &font);

    void sync();

signals:
    void settingsChanged();

private:
    std::unique_ptr<QSettings> m_settings;
};
