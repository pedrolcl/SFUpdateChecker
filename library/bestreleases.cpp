// Copyright (c) 2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QString>
#include <QSysInfo>
#include <QTimeZone>

#include "bestreleases.h"

BestReleases::BestReleases(QObject *parent)
    : QObject{parent}
    , m_currentReply(nullptr)
    , m_project{QStringLiteral(QT_STRINGIFY(PR_PROJECT))}
    , m_current{QVersionNumber::fromString(QT_STRINGIFY(PR_VERSION)),
                QSysInfo::kernelType(),
                QDateTime::fromString(QT_STRINGIFY(PR_DATETIME), Qt::ISODate)}
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &BestReleases::parsingFinished);

    qDebug() << "product:" << QSysInfo::prettyProductName()
             << "product version:" << QSysInfo::productVersion();
    qDebug() << "product type:" << QSysInfo::productType();
    qDebug() << "platform:" << QSysInfo::kernelType()
             << "platform version:" << QSysInfo::kernelVersion();
    qDebug() << "built arch:" << QSysInfo::buildCpuArchitecture()
             << "current arch:" << QSysInfo::currentCpuArchitecture();
    qDebug() << "current release:" << m_current.platform << m_current.version << m_current.title
             << m_current.date;
}

void BestReleases::parseFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        m_json = QJsonDocument::fromJson(file.readAll());
        parseJson();
        emit parsingFinished();
    }
}

void BestReleases::getFromSourceforge()
{
    get(m_urltpl.arg(m_project));
}

void BestReleases::consumeData()
{
    int statusCode = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300) {
        m_json = QJsonDocument::fromJson(m_currentReply->readAll());
        parseJson();
    }
    m_currentReply->disconnect(this);
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void BestReleases::error()
{
    qWarning("error retrieving Json document");
    m_json = QJsonDocument();
    m_currentReply->disconnect(this);
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void BestReleases::get(const QUrl &url)
{
    if (m_currentReply) {
        m_currentReply->disconnect(this);
        m_currentReply->deleteLater();
    }
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent"_ba, "SFUpdateChecker 1.0"_ba);

    m_currentReply = url.isValid() ? m_manager->get(request) : nullptr;
    if (m_currentReply) {
        connect(m_currentReply, &QNetworkReply::readyRead, this, &BestReleases::consumeData);
        connect(m_currentReply, &QNetworkReply::errorOccurred, this, &BestReleases::error);
    }
}

ReleasesList BestReleases::filtered() const
{
    ReleasesList result;
    std::copy_if(cbegin(), cend(), std::back_inserter(result), [=](const ReleaseData &data) {
        return data.version >= m_current.version && data.date > m_current.date
               && data.platform == m_current.platform;
    });
    return result;
}

void BestReleases::parseOneRelease(const QLatin1StringView key, const QJsonObject obj)
{
    ReleaseData item;
    QJsonObject nextObj = obj[key].toObject();
    qint64 size = nextObj["bytes"_L1].toInteger();
    QString title = nextObj["filename"_L1].toString();
    QString pubDate = nextObj["date"_L1].toString();
    QString content = nextObj["mime_type"_L1].toString();
    QString hash = nextObj["md5sum"_L1].toString();
    QString link = key == "release"_L1 ? nextObj["url"_L1].toString()
                                       : m_dwntpl.arg(m_project, title);
    QRegularExpression rex(m_rextpl.arg(m_project));
    auto match = rex.match(title);
    if (match.hasMatch()) {
        item.version = QVersionNumber::fromString(match.captured(1));
    }
    item.platform = key == "release"_L1 ? "source"_L1 : key;
    item.title = title;
    QDateTime dt = QDateTime::fromString(pubDate, m_datefmt);
    dt.setTimeZone(QTimeZone::UTC);
    item.date = dt;
    item.content = content;
    item.size = size;
    item.link = link;
    item.hash = hash.toLatin1();
    push_back(std::move(item));
}

void BestReleases::parseJson()
{
    QJsonObject jsonObject = m_json.object();
    parseOneRelease("release"_L1, jsonObject);

    QJsonObject platforms = jsonObject["platform_releases"_L1].toObject();
    parseOneRelease("mac"_L1, platforms);
    parseOneRelease("windows"_L1, platforms);
    parseOneRelease("linux"_L1, platforms);
}
