#include <QApplication>
#include <QIcon>
#include <QCommandLineParser>
#include "core/Application.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("LinuxShot");
    app.setApplicationDisplayName("LinuxShot");
    app.setOrganizationName("LinuxShot");
    app.setApplicationVersion("0.1.0");
    app.setWindowIcon(QIcon(":/icons/linuxshot.png"));
    app.setQuitOnLastWindowClosed(false);

    QCommandLineParser parser;
    parser.setApplicationDescription("Linux-native screenshot and annotation tool");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption captureOption(QStringList() << "c" << "capture", "Trigger immediate screenshot capture");
    parser.addOption(captureOption);

    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", "Trigger full desktop capture and save");
    parser.addOption(fullscreenOption);

    parser.process(app);

    Application linuxShot;
    if (!linuxShot.init()) {
        return 1;
    }

    if (parser.isSet(captureOption)) {
        linuxShot.triggerCapture();
    } else if (parser.isSet(fullscreenOption)) {
        linuxShot.triggerFullscreenSave();
    }

    return app.exec();
}

