#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QGroupBox>

SettingsDialog::SettingsDialog(SettingsManager *settings, QWidget *parent)
    : QDialog(parent), m_settings(settings)
{
    setWindowTitle("LinuxShot Preferences");
    resize(480, 420);

    auto *mainLayout = new QVBoxLayout(this);
    auto *tabs = new QTabWidget(this);

    tabs->addTab(createGeneralTab(), "General");
    tabs->addTab(createHotkeysTab(), "Hotkeys");
    tabs->addTab(createFormatsTab(), "Formats");
    tabs->addTab(createUploadTab(), "Upload & Network");

    mainLayout->addWidget(tabs);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, this);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        saveSettings();
        accept();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::saveSettings);

    loadSettings();
}

QWidget* SettingsDialog::createGeneralTab() {
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);

    auto *form = new QFormLayout();
    m_langCombo = new QComboBox(widget);
    m_langCombo->addItem("English (en)", "en");
    m_langCombo->addItem("Español (es)", "es");
    m_langCombo->addItem("Deutsch (de)", "de");
    m_langCombo->addItem("Français (fr)", "fr");
    m_langCombo->addItem("Русский (ru)", "ru");
    form->addRow("Language:", m_langCombo);
    layout->addLayout(form);

    auto *grp = new QGroupBox("Options", widget);
    auto *grpLayout = new QVBoxLayout(grp);

    m_showNotifications = new QCheckBox("Show desktop notifications on copy and save", grp);
    m_captureCursor = new QCheckBox("Capture mouse cursor on screenshots", grp);
    m_keepSelection = new QCheckBox("Keep selected area position for subsequent captures", grp);

    grpLayout->addWidget(m_showNotifications);
    grpLayout->addWidget(m_captureCursor);
    grpLayout->addWidget(m_keepSelection);
    layout->addWidget(grp);

    layout->addStretch();
    return widget;
}

QWidget* SettingsDialog::createHotkeysTab() {
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);

    auto *form = new QFormLayout();

    // General Hotkey
    auto *genLayout = new QHBoxLayout();
    m_generalHotkeyEnabled = new QCheckBox(widget);
    m_generalHotkeyEdit = new QKeySequenceEdit(widget);
    genLayout->addWidget(m_generalHotkeyEnabled);
    genLayout->addWidget(m_generalHotkeyEdit);
    form->addRow("General capture hotkey:", genLayout);

    // Save Fullscreen
    auto *saveLayout = new QHBoxLayout();
    m_saveFullHotkeyEnabled = new QCheckBox(widget);
    m_saveFullHotkeyEdit = new QKeySequenceEdit(widget);
    saveLayout->addWidget(m_saveFullHotkeyEnabled);
    saveLayout->addWidget(m_saveFullHotkeyEdit);
    form->addRow("Instant save fullscreen:", saveLayout);

    // Upload Fullscreen
    auto *uplLayout = new QHBoxLayout();
    m_uploadFullHotkeyEnabled = new QCheckBox(widget);
    m_uploadFullHotkeyEdit = new QKeySequenceEdit(widget);
    uplLayout->addWidget(m_uploadFullHotkeyEnabled);
    uplLayout->addWidget(m_uploadFullHotkeyEdit);
    form->addRow("Instant upload fullscreen:", uplLayout);

    layout->addLayout(form);

    auto *infoLabel = new QLabel("Note: On Wayland, global hotkeys require portal authorization or desktop shortcut binding.", widget);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #888888; font-size: 11px; margin-top: 10px;");
    layout->addWidget(infoLabel);

    layout->addStretch();
    return widget;
}

QWidget* SettingsDialog::createFormatsTab() {
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    auto *form = new QFormLayout();

    m_formatCombo = new QComboBox(widget);
    m_formatCombo->addItem("PNG - Lossless Compression (*.png)", "png");
    m_formatCombo->addItem("JPEG - Compressed (*.jpg)", "jpg");
    m_formatCombo->addItem("BMP - Bitmap (*.bmp)", "bmp");
    form->addRow("Default format:", m_formatCombo);

    auto *qualityLayout = new QHBoxLayout();
    m_qualitySlider = new QSlider(Qt::Horizontal, widget);
    m_qualitySlider->setRange(1, 100);
    m_qualityLabel = new QLabel("90%", widget);
    qualityLayout->addWidget(m_qualitySlider);
    qualityLayout->addWidget(m_qualityLabel);
    form->addRow("JPEG quality:", qualityLayout);

    connect(m_qualitySlider, &QSlider::valueChanged, this, [this](int val) {
        m_qualityLabel->setText(QString("%1%").arg(val));
    });

    auto *dirLayout = new QHBoxLayout();
    m_saveDirEdit = new QLineEdit(widget);
    auto *browseBtn = new QPushButton("Browse...", widget);
    connect(browseBtn, &QPushButton::clicked, this, &SettingsDialog::browseSaveDir);
    dirLayout->addWidget(m_saveDirEdit);
    dirLayout->addWidget(browseBtn);
    form->addRow("Default save directory:", dirLayout);

    layout->addLayout(form);
    layout->addStretch();
    return widget;
}

QWidget* SettingsDialog::createUploadTab() {
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    auto *form = new QFormLayout();

    m_uploadEndpointEdit = new QLineEdit(widget);
    m_uploadEndpointEdit->setPlaceholderText("https://your-server.com/api/upload");
    form->addRow("Upload Endpoint URL:", m_uploadEndpointEdit);

    m_autoCopyLink = new QCheckBox("Automatically copy resulting link to clipboard", widget);
    m_autoCloseWindow = new QCheckBox("Automatically close upload window on success", widget);
    form->addRow("", m_autoCopyLink);
    form->addRow("", m_autoCloseWindow);

    layout->addLayout(form);

    auto *proxyGrp = new QGroupBox("Proxy Settings", widget);
    auto *pForm = new QFormLayout(proxyGrp);
    m_proxyCombo = new QComboBox(proxyGrp);
    m_proxyCombo->addItem("No proxy (direct connection)", 0);
    m_proxyCombo->addItem("Use system proxy settings", 1);
    m_proxyCombo->addItem("Manual proxy configuration", 2);
    pForm->addRow("Proxy type:", m_proxyCombo);

    m_proxyHostEdit = new QLineEdit(proxyGrp);
    m_proxyHostEdit->setPlaceholderText("127.0.0.1");
    pForm->addRow("Proxy Host:", m_proxyHostEdit);

    m_proxyPortEdit = new QLineEdit(proxyGrp);
    m_proxyPortEdit->setPlaceholderText("8080");
    pForm->addRow("Proxy Port:", m_proxyPortEdit);

    layout->addWidget(proxyGrp);
    layout->addStretch();
    return widget;
}

void SettingsDialog::browseSaveDir() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Default Directory", m_saveDirEdit->text());
    if (!dir.isEmpty()) {
        m_saveDirEdit->setText(dir);
    }
}

void SettingsDialog::loadSettings() {
    if (!m_settings) return;

    // General
    int langIdx = m_langCombo->findData(m_settings->language());
    if (langIdx >= 0) m_langCombo->setCurrentIndex(langIdx);
    m_showNotifications->setChecked(m_settings->showNotifications());
    m_captureCursor->setChecked(m_settings->captureCursor());
    m_keepSelection->setChecked(m_settings->keepSelection());

    // Hotkeys
    m_generalHotkeyEnabled->setChecked(m_settings->generalHotkeyEnabled());
    m_generalHotkeyEdit->setKeySequence(QKeySequence(m_settings->generalHotkey()));

    m_saveFullHotkeyEnabled->setChecked(m_settings->saveFullscreenHotkeyEnabled());
    m_saveFullHotkeyEdit->setKeySequence(QKeySequence(m_settings->saveFullscreenHotkey()));

    m_uploadFullHotkeyEnabled->setChecked(m_settings->uploadFullscreenHotkeyEnabled());
    m_uploadFullHotkeyEdit->setKeySequence(QKeySequence(m_settings->uploadFullscreenHotkey()));

    // Formats
    int fmtIdx = m_formatCombo->findData(m_settings->defaultFormat());
    if (fmtIdx >= 0) m_formatCombo->setCurrentIndex(fmtIdx);
    m_qualitySlider->setValue(m_settings->jpegQuality());
    m_qualityLabel->setText(QString("%1%").arg(m_settings->jpegQuality()));
    m_saveDirEdit->setText(m_settings->defaultSaveDir());

    // Upload & Network
    m_uploadEndpointEdit->setText(m_settings->uploadEndpoint());
    m_autoCopyLink->setChecked(m_settings->autoCopyLink());
    m_autoCloseWindow->setChecked(m_settings->autoCloseUploadWindow());

    int proxyIdx = m_proxyCombo->findData(m_settings->proxyType());
    if (proxyIdx >= 0) m_proxyCombo->setCurrentIndex(proxyIdx);
    m_proxyHostEdit->setText(m_settings->proxyHost());
    m_proxyPortEdit->setText(QString::number(m_settings->proxyPort()));
}

void SettingsDialog::saveSettings() {
    if (!m_settings) return;

    // General
    m_settings->setLanguage(m_langCombo->currentData().toString());
    m_settings->setShowNotifications(m_showNotifications->isChecked());
    m_settings->setCaptureCursor(m_captureCursor->isChecked());
    m_settings->setKeepSelection(m_keepSelection->isChecked());

    // Hotkeys
    m_settings->setGeneralHotkeyEnabled(m_generalHotkeyEnabled->isChecked());
    m_settings->setGeneralHotkey(m_generalHotkeyEdit->keySequence().toString());

    m_settings->setSaveFullscreenHotkeyEnabled(m_saveFullHotkeyEnabled->isChecked());
    m_settings->setSaveFullscreenHotkey(m_saveFullHotkeyEdit->keySequence().toString());

    m_settings->setUploadFullscreenHotkeyEnabled(m_uploadFullHotkeyEnabled->isChecked());
    m_settings->setUploadFullscreenHotkey(m_uploadFullHotkeyEdit->keySequence().toString());

    // Formats
    m_settings->setDefaultFormat(m_formatCombo->currentData().toString());
    m_settings->setJpegQuality(m_qualitySlider->value());
    m_settings->setDefaultSaveDir(m_saveDirEdit->text());

    // Upload & Network
    m_settings->setUploadEndpoint(m_uploadEndpointEdit->text().trimmed());
    m_settings->setAutoCopyLink(m_autoCopyLink->isChecked());
    m_settings->setAutoCloseUploadWindow(m_autoCloseWindow->isChecked());

    m_settings->setProxyType(m_proxyCombo->currentData().toInt());
    m_settings->setProxyHost(m_proxyHostEdit->text().trimmed());
    m_settings->setProxyPort(m_proxyPortEdit->text().toInt());

    m_settings->sync();
}
