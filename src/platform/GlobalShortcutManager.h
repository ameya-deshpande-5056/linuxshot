#pragma once

#include <QObject>
#include <QKeySequence>
#include <QMap>
#include <QAbstractNativeEventFilter>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

class GlobalShortcutManager : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT

public:
    explicit GlobalShortcutManager(QObject *parent = nullptr);
    ~GlobalShortcutManager() override;

    bool registerShortcut(const QString &id, const QKeySequence &sequence);
    void unregisterShortcut(const QString &id);
    void unregisterAll();

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

signals:
    void shortcutActivated(const QString &id);
    void registrationFailed(const QString &id, const QString &error);

private:
    struct ShortcutData {
        xcb_keycode_t keycode = 0;
        uint16_t modifiers = 0;
        QKeySequence sequence;
    };

    bool initXcb();
    void cleanupXcb();
    bool parseKeySequence(const QKeySequence &seq, xcb_keycode_t &outCode, uint16_t &outMods);

    xcb_connection_t *m_connection = nullptr;
    xcb_window_t m_rootWindow = 0;
    xcb_key_symbols_t *m_keySymbols = nullptr;
    bool m_isX11 = false;

    QMap<QString, ShortcutData> m_shortcuts;
    QMap<QPair<xcb_keycode_t, uint16_t>, QString> m_lookup;
};

