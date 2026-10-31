#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QApplication>
#include <QCoreApplication>

#ifndef LINUXSHOT_BUILD_TIMESTAMP
#define LINUXSHOT_BUILD_TIMESTAMP "2026-10-31T00:00:00Z"
#endif

#ifndef LINUXSHOT_PROJECT_URL
#define LINUXSHOT_PROJECT_URL "https://github.com/ameya-deshpande-5056/linuxshot"
#endif

#ifndef LINUXSHOT_COMMIT_HASH
#define LINUXSHOT_COMMIT_HASH "unknown"
#endif

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("About LinuxShot"));
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 18);
    layout->setSpacing(20);

    auto *content = new QHBoxLayout;
    content->setSpacing(28);

    auto *logo = new QLabel(this);
    logo->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    QPixmap iconPixmap = QApplication::windowIcon().pixmap(112, 112);
    if (iconPixmap.isNull()) {
        iconPixmap = QPixmap(":/icons/linuxshot.png").scaled(112, 112, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    logo->setPixmap(iconPixmap);
    logo->setFixedSize(112, 112);
    logo->setScaledContents(true);
    logo->setAccessibleName(QStringLiteral("LinuxShot logo"));
    content->addWidget(logo, 0, Qt::AlignTop);

    auto *text = new QVBoxLayout;
    text->setSpacing(7);

    QString appVer = QCoreApplication::applicationVersion();
    if (appVer.isEmpty()) {
#ifdef LINUXSHOT_VERSION
        appVer = QStringLiteral(LINUXSHOT_VERSION);
#else
        appVer = QStringLiteral("0.1.1");
#endif
    }

    auto *title = new QLabel(QStringLiteral("LinuxShot %1").arg(appVer), this);
    auto titleFont = title->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 4);
    title->setFont(titleFont);
    text->addWidget(title);

    auto *description = new QLabel(
        QStringLiteral("A Linux-native Qt screenshot and annotation tool."), this);
    description->setWordWrap(true);
    text->addWidget(description);

    auto *details = new QFormLayout;
    details->setContentsMargins(0, 5, 0, 0);
    details->setHorizontalSpacing(14);
    details->setVerticalSpacing(7);
    details->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    details->addRow(QStringLiteral("Version:"), new QLabel(appVer, this));

    auto *commitLabel = new QLabel(this);
    if (QStringLiteral(LINUXSHOT_COMMIT_HASH) != QStringLiteral("unknown")) {
        commitLabel->setText(QStringLiteral("<a href=\"%1/commit/%2\">%2</a>")
            .arg(QStringLiteral(LINUXSHOT_PROJECT_URL), QStringLiteral(LINUXSHOT_COMMIT_HASH)));
        commitLabel->setTextFormat(Qt::RichText);
        commitLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
        commitLabel->setOpenExternalLinks(true);
    } else {
        commitLabel->setText(QStringLiteral("unknown"));
    }
    details->addRow(QStringLiteral("Commit:"), commitLabel);

    details->addRow(QStringLiteral("Build time:"),
                    new QLabel(QStringLiteral(LINUXSHOT_BUILD_TIMESTAMP), this));
    details->addRow(QStringLiteral("Qt runtime:"),
                    new QLabel(QString::fromLatin1(qVersion()), this));

    auto *projectLink = new QLabel(
        QStringLiteral("<a href=\"%1\">%1</a>").arg(QStringLiteral(LINUXSHOT_PROJECT_URL)), this);
    projectLink->setTextFormat(Qt::RichText);
    projectLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    projectLink->setOpenExternalLinks(true);
    details->addRow(QStringLiteral("Project:"), projectLink);

    text->addLayout(details);
    text->addStretch();
    content->addLayout(text, 1);
    layout->addLayout(content);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    setMinimumWidth(600);
}

