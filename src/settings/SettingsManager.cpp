#include "SettingsManager.h"
#include <QStandardPaths>
#include <QDir>

SettingsManager::SettingsManager(QObject *parent)
    : SettingsManager(QString(), parent)
{
}

SettingsManager::SettingsManager(const QString &customConfigPath, QObject *parent)
    : QObject(parent)
{
    QString configPath = customConfigPath;
    if (configPath.isEmpty()) {
        const QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        QDir().mkpath(configDir);
        configPath = configDir + "/linuxshot.conf";
    }
    m_settings = std::make_unique<QSettings>(configPath, QSettings::IniFormat);
}

// Hotkeys
QString SettingsManager::generalHotkey() const {
    return m_settings->value("hotkeys/general", "Print").toString();
}

void SettingsManager::setGeneralHotkey(const QString &key) {
    m_settings->setValue("hotkeys/general", key);
    emit settingsChanged();
}

bool SettingsManager::generalHotkeyEnabled() const {
    return m_settings->value("hotkeys/general_enabled", true).toBool();
}

void SettingsManager::setGeneralHotkeyEnabled(bool enabled) {
    m_settings->setValue("hotkeys/general_enabled", enabled);
    emit settingsChanged();
}

QString SettingsManager::saveFullscreenHotkey() const {
    return m_settings->value("hotkeys/save_fullscreen", "Shift+Print").toString();
}

void SettingsManager::setSaveFullscreenHotkey(const QString &key) {
    m_settings->setValue("hotkeys/save_fullscreen", key);
    emit settingsChanged();
}

bool SettingsManager::saveFullscreenHotkeyEnabled() const {
    return m_settings->value("hotkeys/save_fullscreen_enabled", true).toBool();
}

void SettingsManager::setSaveFullscreenHotkeyEnabled(bool enabled) {
    m_settings->setValue("hotkeys/save_fullscreen_enabled", enabled);
    emit settingsChanged();
}

QString SettingsManager::uploadFullscreenHotkey() const {
    return m_settings->value("hotkeys/upload_fullscreen", "").toString();
}

void SettingsManager::setUploadFullscreenHotkey(const QString &key) {
    m_settings->setValue("hotkeys/upload_fullscreen", key);
    emit settingsChanged();
}

bool SettingsManager::uploadFullscreenHotkeyEnabled() const {
    return m_settings->value("hotkeys/upload_fullscreen_enabled", false).toBool();
}

void SettingsManager::setUploadFullscreenHotkeyEnabled(bool enabled) {
    m_settings->setValue("hotkeys/upload_fullscreen_enabled", enabled);
    emit settingsChanged();
}

// General
QString SettingsManager::language() const {
    return m_settings->value("general/language", "en").toString();
}

void SettingsManager::setLanguage(const QString &lang) {
    m_settings->setValue("general/language", lang);
    emit settingsChanged();
}

bool SettingsManager::showNotifications() const {
    return m_settings->value("general/show_notifications", true).toBool();
}

void SettingsManager::setShowNotifications(bool show) {
    m_settings->setValue("general/show_notifications", show);
    emit settingsChanged();
}

bool SettingsManager::captureCursor() const {
    return m_settings->value("general/capture_cursor", false).toBool();
}

void SettingsManager::setCaptureCursor(bool capture) {
    m_settings->setValue("general/capture_cursor", capture);
    emit settingsChanged();
}

bool SettingsManager::keepSelection() const {
    return m_settings->value("general/keep_selection", false).toBool();
}

void SettingsManager::setKeepSelection(bool keep) {
    m_settings->setValue("general/keep_selection", keep);
    emit settingsChanged();
}

QRect SettingsManager::lastSelection() const {
    return m_settings->value("general/last_selection", QRect()).toRect();
}

void SettingsManager::setLastSelection(const QRect &rect) {
    m_settings->setValue("general/last_selection", rect);
}

// Formats & Save
QString SettingsManager::defaultSaveDir() const {
    const QString pictures = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    return m_settings->value("formats/default_save_dir", pictures).toString();
}

void SettingsManager::setDefaultSaveDir(const QString &dir) {
    m_settings->setValue("formats/default_save_dir", dir);
    emit settingsChanged();
}

QString SettingsManager::defaultFormat() const {
    return m_settings->value("formats/default_format", "png").toString().toLower();
}

void SettingsManager::setDefaultFormat(const QString &fmt) {
    m_settings->setValue("formats/default_format", fmt.toLower());
    emit settingsChanged();
}

int SettingsManager::jpegQuality() const {
    return m_settings->value("formats/jpeg_quality", 90).toInt();
}

void SettingsManager::setJpegQuality(int quality) {
    m_settings->setValue("formats/jpeg_quality", qBound(1, quality, 100));
    emit settingsChanged();
}

// Upload & Network
QString SettingsManager::uploadEndpoint() const {
    return m_settings->value("upload/endpoint", "").toString();
}

void SettingsManager::setUploadEndpoint(const QString &url) {
    m_settings->setValue("upload/endpoint", url);
    emit settingsChanged();
}

bool SettingsManager::autoCopyLink() const {
    return m_settings->value("upload/auto_copy_link", true).toBool();
}

void SettingsManager::setAutoCopyLink(bool autoCopy) {
    m_settings->setValue("upload/auto_copy_link", autoCopy);
    emit settingsChanged();
}

bool SettingsManager::autoCloseUploadWindow() const {
    return m_settings->value("upload/auto_close_window", true).toBool();
}

void SettingsManager::setAutoCloseUploadWindow(bool autoClose) {
    m_settings->setValue("upload/auto_close_window", autoClose);
    emit settingsChanged();
}

int SettingsManager::proxyType() const {
    return m_settings->value("network/proxy_type", 0).toInt();
}

void SettingsManager::setProxyType(int type) {
    m_settings->setValue("network/proxy_type", type);
    emit settingsChanged();
}

QString SettingsManager::proxyHost() const {
    return m_settings->value("network/proxy_host", "").toString();
}

void SettingsManager::setProxyHost(const QString &host) {
    m_settings->setValue("network/proxy_host", host);
    emit settingsChanged();
}

int SettingsManager::proxyPort() const {
    return m_settings->value("network/proxy_port", 8080).toInt();
}

void SettingsManager::setProxyPort(int port) {
    m_settings->setValue("network/proxy_port", port);
    emit settingsChanged();
}

// Annotation tool defaults
QColor SettingsManager::penColor() const {
    return QColor(m_settings->value("annotation/pen_color", "#FF0000").toString());
}

void SettingsManager::setPenColor(const QColor &color) {
    m_settings->setValue("annotation/pen_color", color.name(QColor::HexArgb));
    emit settingsChanged();
}

int SettingsManager::penWidth() const {
    return m_settings->value("annotation/pen_width", 3).toInt();
}

void SettingsManager::setPenWidth(int width) {
    m_settings->setValue("annotation/pen_width", qBound(1, width, 50));
    emit settingsChanged();
}

int SettingsManager::markerWidth() const {
    return m_settings->value("annotation/marker_width", 20).toInt();
}

void SettingsManager::setMarkerWidth(int width) {
    m_settings->setValue("annotation/marker_width", qBound(5, width, 100));
    emit settingsChanged();
}

QFont SettingsManager::textFont() const {
    QFont f;
    f.fromString(m_settings->value("annotation/text_font", QFont("Sans Serif", 14, QFont::Bold).toString()).toString());
    return f;
}

void SettingsManager::setTextFont(const QFont &font) {
    m_settings->setValue("annotation/text_font", font.toString());
    emit settingsChanged();
}

void SettingsManager::sync() {
    m_settings->sync();
}
