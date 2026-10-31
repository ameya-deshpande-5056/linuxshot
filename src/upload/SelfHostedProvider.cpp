#include "SelfHostedProvider.h"
#include <QHttpMultiPart>
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>

SelfHostedProvider::SelfHostedProvider(QObject *parent)
    : UploadProvider(parent)
{
    connect(&m_netManager, &QNetworkAccessManager::finished, this, &SelfHostedProvider::onUploadFinished);
}

bool SelfHostedProvider::isConfigured() const {
    return !m_endpoint.trimmed().isEmpty();
}

void SelfHostedProvider::upload(const QImage &image, const UploadOptions &opts) {
    QString targetEndpoint = opts.customEndpoint.isEmpty() ? m_endpoint : opts.customEndpoint;

    if (targetEndpoint.trimmed().isEmpty()) {
        UploadResult res;
        res.success = false;
        res.errorMessage = "Upload endpoint is not configured in Preferences.";
        emit uploadCompleted(res);
        return;
    }

    if (image.isNull()) {
        UploadResult res;
        res.success = false;
        res.errorMessage = "Cannot upload empty image.";
        emit uploadCompleted(res);
        return;
    }

    // Convert image to byte array in selected format
    QByteArray imageBytes;
    QBuffer buffer(&imageBytes);
    buffer.open(QIODevice::WriteOnly);

    QString mimeType = "image/png";
    const char *formatStr = "PNG";
    if (opts.format.toLower() == "jpg" || opts.format.toLower() == "jpeg") {
        mimeType = "image/jpeg";
        formatStr = "JPG";
        image.save(&buffer, formatStr, opts.jpegQuality);
    } else {
        image.save(&buffer, formatStr);
    }

    auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString("form-data; name=\"image\"; filename=\"screenshot.%1\"")
                                 .arg(opts.format.toLower())));
    imagePart.setBody(imageBytes);
    multiPart->append(imagePart);

    QUrl url(targetEndpoint);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "LinuxShot/0.1.0 (Linux; x86_64)");

    QNetworkReply *reply = m_netManager.post(request, multiPart);
    multiPart->setParent(reply); // ownership transferred to reply

    connect(reply, &QNetworkReply::uploadProgress, this, &UploadProvider::uploadProgress);
}

void SelfHostedProvider::onUploadFinished(QNetworkReply *reply) {
    UploadResult result;

    if (reply->error() != QNetworkReply::NoError) {
        result.success = false;
        result.errorMessage = reply->errorString();
    } else {
        QByteArray responseData = reply->readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject obj = doc.object();
            // Check common response keys: "url", "link", or nested {"data": {"url": "..."}}
            QString urlStr;
            if (obj.contains("url")) {
                urlStr = obj["url"].toString();
            } else if (obj.contains("link")) {
                urlStr = obj["link"].toString();
            } else if (obj.contains("data") && obj["data"].isObject()) {
                QJsonObject dataObj = obj["data"].toObject();
                urlStr = dataObj["url"].toString();
                if (urlStr.isEmpty()) urlStr = dataObj["link"].toString();
            }

            if (!urlStr.isEmpty()) {
                result.success = true;
                result.imageUrl = QUrl(urlStr);
            } else {
                result.success = false;
                result.errorMessage = "Invalid JSON structure in server response: no 'url' field found.";
            }
        } else {
            // Raw URL string in plaintext response
            QString plain = QString::fromUtf8(responseData).trimmed();
            if (plain.startsWith("http://") || plain.startsWith("https://")) {
                result.success = true;
                result.imageUrl = QUrl(plain);
            } else {
                result.success = false;
                result.errorMessage = "Unrecognized response format from server.";
            }
        }
    }

    reply->deleteLater();
    emit uploadCompleted(result);
}
