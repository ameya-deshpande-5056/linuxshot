#include "PrntscrProvider.h"
#include <QHttpMultiPart>
#include <QBuffer>
#include <QDateTime>
#include <QCryptographicHash>
#include <QUuid>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>

PrntscrProvider::PrntscrProvider(QObject *parent)
    : UploadProvider(parent)
{
    connect(&m_netManager, &QNetworkAccessManager::finished, this, &PrntscrProvider::onUploadFinished);
}

QString PrntscrProvider::generateToken() {
    // Legacy helper; when unauthenticated, Lightshot omits app_token
    return "37162342db68031b";
}

QString PrntscrProvider::generateHash(qint64 timestamp) {
    // Lightshot binaries store the base64 encrypted string "gEI/L0Dw2yCeZXFeNsOapQ=="
    // and decrypt it at runtime using AES-128-CBC into the 12-character salt "5CE3DF4D45AC".
    const QString salt = "5CE3DF4D45AC";
    QString toHash = QString("%1*%2").arg(salt).arg(timestamp);
    return QCryptographicHash::hash(toHash.toUtf8(), QCryptographicHash::Md5).toHex();
}

void PrntscrProvider::upload(const QImage &image, const UploadOptions &opts) {
    Q_UNUSED(opts);

    if (image.isNull()) {
        UploadResult res;
        res.success = false;
        res.errorMessage = "Cannot upload empty image.";
        emit uploadCompleted(res);
        return;
    }

    QByteArray imageBytes;
    QBuffer buffer(&imageBytes);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");

    qint64 timestamp = QDateTime::currentSecsSinceEpoch();
    QString hash = generateHash(timestamp);
    QString urlStr = QString("https://upload.prntscr.com/upload/%1/%2/").arg(timestamp).arg(hash);

    auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // 1. image
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"image\"; filename=\"screenshot.png\""));
    imagePart.setBody(imageBytes);
    multiPart->append(imagePart);

    // 2. direct_link
    QHttpPart directLinkPart;
    directLinkPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"direct_link\""));
    directLinkPart.setBody("True");
    multiPart->append(directLinkPart);

    // 3. width
    QHttpPart widthPart;
    widthPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"width\""));
    widthPart.setBody(QByteArray::number(image.width()));
    multiPart->append(widthPart);

    // 4. height
    QHttpPart heightPart;
    heightPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"height\""));
    heightPart.setBody(QByteArray::number(image.height()));
    multiPart->append(heightPart);

    // 5. dpi
    QHttpPart dpiPart;
    dpiPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"dpi\""));
    qreal dpiVal = image.logicalDpiX() > 0 ? image.logicalDpiX() : 96.0;
    dpiPart.setBody(QByteArray::number(dpiVal, 'f', 6));
    multiPart->append(dpiPart);

    // 6. app_id
    QHttpPart appIdPart;
    appIdPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"app_id\""));
    appIdPart.setBody(QUuid::createUuid().toString(QUuid::WithBraces).toUpper().toUtf8());
    multiPart->append(appIdPart);

    QUrl url(urlStr);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.100 Safari/537.36");

    QNetworkReply *reply = m_netManager.post(request, multiPart);
    multiPart->setParent(reply);

    connect(reply, &QNetworkReply::uploadProgress, this, &UploadProvider::uploadProgress);
}

void PrntscrProvider::onUploadFinished(QNetworkReply *reply) {
    UploadResult result;
    if (reply->error() != QNetworkReply::NoError) {
        result.success = false;
        result.errorMessage = reply->errorString();
    } else {
        QByteArray responseData = reply->readAll();
        QString responseText = QString::fromUtf8(responseData);

        // Check for XML response: <response><status>success</status><share>...</share></response>
        QRegularExpression statusRegex("<status>([^<]+)</status>", QRegularExpression::CaseInsensitiveOption);
        auto statusMatch = statusRegex.match(responseText);

        if (statusMatch.hasMatch() && statusMatch.captured(1).trimmed().compare("success", Qt::CaseInsensitive) == 0) {
            QRegularExpression shareRegex("<share>([^<]+)</share>", QRegularExpression::CaseInsensitiveOption);
            auto shareMatch = shareRegex.match(responseText);
            if (shareMatch.hasMatch()) {
                QString urlStr = shareMatch.captured(1).trimmed();
                if (urlStr.startsWith("http://", Qt::CaseInsensitive) || urlStr.startsWith("https://", Qt::CaseInsensitive)) {
                    result.success = true;
                    result.imageUrl = QUrl(urlStr);
                } else {
                    result.success = false;
                    result.errorMessage = "Invalid share URL received from Prntscr.";
                }
            } else {
                result.success = false;
                result.errorMessage = "Prntscr response missing share URL.";
            }
        } else if (statusMatch.hasMatch()) {
            QRegularExpression msgRegex("<message>([^<]+)</message>", QRegularExpression::CaseInsensitiveOption);
            auto msgMatch = msgRegex.match(responseText);
            QString errMsg = msgMatch.hasMatch() ? msgMatch.captured(1).trimmed() : "Unknown error";
            result.success = false;
            result.errorMessage = QString("Prntscr upload failed: %1").arg(errMsg);
        } else {
            // Check for JSON response format
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
            if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj.value("status").toString().compare("success", Qt::CaseInsensitive) == 0) {
                    QString urlStr = obj.value("share").toString();
                    if (urlStr.isEmpty()) urlStr = obj.value("url").toString();
                    if (!urlStr.isEmpty()) {
                        result.success = true;
                        result.imageUrl = QUrl(urlStr);
                    } else {
                        result.success = false;
                        result.errorMessage = "Missing share URL in Prntscr JSON response.";
                    }
                } else {
                    QString errMsg = obj.value("message").toString("Unknown error");
                    result.success = false;
                    result.errorMessage = QString("Prntscr upload failed: %1").arg(errMsg);
                }
            } else {
                result.success = false;
                result.errorMessage = "Failed to parse Prntscr server response.";
            }
        }
    }
    reply->deleteLater();
    emit uploadCompleted(result);
}

