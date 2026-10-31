#pragma once

#include "CaptureBackend.h"
#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <sys/ipc.h>
#include <sys/shm.h>

class X11CaptureBackend : public CaptureBackend {
public:
    X11CaptureBackend();
    ~X11CaptureBackend() override;

    QString name() const override { return "X11/XCB-SHM"; }
    bool isAvailable() const override;

    QImage captureScreen(int screenIndex = -1) override;
    QImage captureFullDesktop() override;
    QList<QRect> screenGeometries() override;

private:
    bool initShm();
    void cleanupShm();
    QImage captureViaShm(const QRect &geometry);

    xcb_connection_t *m_connection = nullptr;
    xcb_window_t m_rootWindow = 0;
    xcb_shm_seg_t m_shmSeg = 0;
    int m_shmId = -1;
    uint8_t *m_shmAddr = nullptr;
    size_t m_shmSize = 0;
    bool m_shmAvailable = false;
};
