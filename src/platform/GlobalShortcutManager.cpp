#include "GlobalShortcutManager.h"
#include <QGuiApplication>
#include <QDebug>
#include <X11/keysym.h>

GlobalShortcutManager::GlobalShortcutManager(QObject *parent)
    : QObject(parent)
{
    m_isX11 = (QGuiApplication::platformName().compare("xcb", Qt::CaseInsensitive) == 0);
    if (m_isX11) {
        if (initXcb()) {
            qApp->installNativeEventFilter(this);
        }
    }
}

GlobalShortcutManager::~GlobalShortcutManager() {
    if (m_isX11) {
        unregisterAll();
        qApp->removeNativeEventFilter(this);
        cleanupXcb();
    }
}

bool GlobalShortcutManager::initXcb() {
    m_connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(m_connection)) {
        cleanupXcb();
        return false;
    }

    const xcb_setup_t *setup = xcb_get_setup(m_connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    if (iter.rem > 0) {
        m_rootWindow = iter.data->root;
    } else {
        cleanupXcb();
        return false;
    }

    m_keySymbols = xcb_key_symbols_alloc(m_connection);
    return (m_keySymbols != nullptr);
}

void GlobalShortcutManager::cleanupXcb() {
    if (m_keySymbols) {
        xcb_key_symbols_free(m_keySymbols);
        m_keySymbols = nullptr;
    }
    if (m_connection) {
        xcb_disconnect(m_connection);
        m_connection = nullptr;
    }
}

bool GlobalShortcutManager::parseKeySequence(const QKeySequence &seq, xcb_keycode_t &outCode, uint16_t &outMods) {
    if (seq.isEmpty() || !m_keySymbols) {
        return false;
    }

    int keyCombo = seq[0].toCombined();
    int key = keyCombo & ~Qt::KeyboardModifierMask;
    Qt::KeyboardModifiers qmods = Qt::KeyboardModifiers(keyCombo & Qt::KeyboardModifierMask);

    outMods = 0;
    if (qmods & Qt::ShiftModifier)   outMods |= XCB_MOD_MASK_SHIFT;
    if (qmods & Qt::ControlModifier) outMods |= XCB_MOD_MASK_CONTROL;
    if (qmods & Qt::AltModifier)     outMods |= XCB_MOD_MASK_1;
    if (qmods & Qt::MetaModifier)    outMods |= XCB_MOD_MASK_4;

    xcb_keysym_t sym = 0;
    switch (key) {
        case Qt::Key_Print:     sym = XK_Print; break;
        case Qt::Key_Escape:    sym = XK_Escape; break;
        case Qt::Key_Return:    sym = XK_Return; break;
        case Qt::Key_Enter:     sym = XK_KP_Enter; break;
        case Qt::Key_Space:     sym = XK_space; break;
        case Qt::Key_F1:        sym = XK_F1; break;
        case Qt::Key_F2:        sym = XK_F2; break;
        case Qt::Key_F3:        sym = XK_F3; break;
        case Qt::Key_F4:        sym = XK_F4; break;
        case Qt::Key_F5:        sym = XK_F5; break;
        case Qt::Key_F6:        sym = XK_F6; break;
        case Qt::Key_F7:        sym = XK_F7; break;
        case Qt::Key_F8:        sym = XK_F8; break;
        case Qt::Key_F9:        sym = XK_F9; break;
        case Qt::Key_F10:       sym = XK_F10; break;
        case Qt::Key_F11:       sym = XK_F11; break;
        case Qt::Key_F12:       sym = XK_F12; break;
        default:
            if (key >= Qt::Key_A && key <= Qt::Key_Z) {
                sym = XK_a + (key - Qt::Key_A);
            } else if (key >= Qt::Key_0 && key <= Qt::Key_9) {
                sym = XK_0 + (key - Qt::Key_0);
            } else {
                sym = key;
            }
            break;
    }

    xcb_keycode_t *keycodes = xcb_key_symbols_get_keycode(m_keySymbols, sym);
    if (!keycodes || keycodes[0] == 0) {
        free(keycodes);
        return false;
    }

    outCode = keycodes[0];
    free(keycodes);
    return true;
}

bool GlobalShortcutManager::registerShortcut(const QString &id, const QKeySequence &sequence) {
    if (!m_isX11 || !m_connection || sequence.isEmpty()) {
        return false;
    }

    unregisterShortcut(id);

    xcb_keycode_t keycode = 0;
    uint16_t mods = 0;
    if (!parseKeySequence(sequence, keycode, mods)) {
        emit registrationFailed(id, "Could not map shortcut key sequence");
        return false;
    }

    // Grab key with and without NumLock/CapsLock/ScrollLock
    const uint16_t ignoredMasks[] = {
        0,
        XCB_MOD_MASK_2,                 // NumLock
        XCB_MOD_MASK_LOCK,              // CapsLock
        XCB_MOD_MASK_2 | XCB_MOD_MASK_LOCK
    };

    bool anyGrabbed = false;
    for (uint16_t ign : ignoredMasks) {
        xcb_void_cookie_t cookie = xcb_grab_key_checked(
            m_connection, 1, m_rootWindow, mods | ign, keycode,
            XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC
        );
        xcb_generic_error_t *error = xcb_request_check(m_connection, cookie);
        if (!error) {
            anyGrabbed = true;
        } else {
            free(error);
        }
    }
    xcb_flush(m_connection);

    if (!anyGrabbed) {
        emit registrationFailed(id, "Shortcut is already in use by another application");
        return false;
    }

    ShortcutData data;
    data.keycode = keycode;
    data.modifiers = mods;
    data.sequence = sequence;

    m_shortcuts[id] = data;
    m_lookup[{keycode, mods}] = id;
    return true;
}

void GlobalShortcutManager::unregisterShortcut(const QString &id) {
    if (!m_shortcuts.contains(id) || !m_isX11 || !m_connection) {
        return;
    }

    const ShortcutData &data = m_shortcuts[id];
    const uint16_t ignoredMasks[] = {
        0, XCB_MOD_MASK_2, XCB_MOD_MASK_LOCK, XCB_MOD_MASK_2 | XCB_MOD_MASK_LOCK
    };

    for (uint16_t ign : ignoredMasks) {
        xcb_ungrab_key(m_connection, data.keycode, m_rootWindow, data.modifiers | ign);
    }
    xcb_flush(m_connection);

    m_lookup.remove({data.keycode, data.modifiers});
    m_shortcuts.remove(id);
}

void GlobalShortcutManager::unregisterAll() {
    for (const QString &id : m_shortcuts.keys()) {
        unregisterShortcut(id);
    }
}

bool GlobalShortcutManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) {
    Q_UNUSED(result);
    if (!m_isX11 || eventType != "xcb_generic_event_t") {
        return false;
    }

    auto *event = static_cast<xcb_generic_event_t *>(message);
    uint8_t responseType = event->response_type & ~0x80;

    if (responseType == XCB_KEY_PRESS) {
        auto *keyEvent = reinterpret_cast<xcb_key_press_event_t *>(event);
        uint16_t mods = keyEvent->state & (XCB_MOD_MASK_SHIFT | XCB_MOD_MASK_CONTROL | XCB_MOD_MASK_1 | XCB_MOD_MASK_4);
        xcb_keycode_t code = keyEvent->detail;

        QPair<xcb_keycode_t, uint16_t> keyPair = {code, mods};
        if (m_lookup.contains(keyPair)) {
            QString id = m_lookup.value(keyPair);
            emit shortcutActivated(id);
            return true;
        }
    }

    return false;
}

