#pragma once

#include <QDialog>
#include <QTabWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QKeySequenceEdit>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include "settings/SettingsManager.h"

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(SettingsManager *settings, QWidget *parent = nullptr);
    ~SettingsDialog() override = default;

private slots:
    void loadSettings();
    void saveSettings();
    void browseSaveDir();

private:
    QWidget* createGeneralTab();
    QWidget* createHotkeysTab();
    QWidget* createFormatsTab();
    QWidget* createUploadTab();

    SettingsManager *m_settings = nullptr;

    // General widgets
    QComboBox *m_langCombo = nullptr;
    QCheckBox *m_showNotifications = nullptr;
    QCheckBox *m_captureCursor = nullptr;
    QCheckBox *m_keepSelection = nullptr;

    // Hotkeys widgets
    QCheckBox *m_generalHotkeyEnabled = nullptr;
    QKeySequenceEdit *m_generalHotkeyEdit = nullptr;
    QCheckBox *m_saveFullHotkeyEnabled = nullptr;
    QKeySequenceEdit *m_saveFullHotkeyEdit = nullptr;
    QCheckBox *m_uploadFullHotkeyEnabled = nullptr;
    QKeySequenceEdit *m_uploadFullHotkeyEdit = nullptr;

    // Formats widgets
    QComboBox *m_formatCombo = nullptr;
    QSlider *m_qualitySlider = nullptr;
    QLabel *m_qualityLabel = nullptr;
    QLineEdit *m_saveDirEdit = nullptr;

    // Network / Upload widgets
    QLineEdit *m_uploadEndpointEdit = nullptr;
    QCheckBox *m_autoCopyLink = nullptr;
    QCheckBox *m_autoCloseWindow = nullptr;
    QComboBox *m_proxyCombo = nullptr;
    QLineEdit *m_proxyHostEdit = nullptr;
    QLineEdit *m_proxyPortEdit = nullptr;
};

