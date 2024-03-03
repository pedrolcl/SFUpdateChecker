// Copyright (c) 2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QString>
#include <QSysInfo>
#include <QXmlStreamReader>

#include "releasestable.h"

using namespace Qt::Literals::StringLiterals;

ReleasesTable::ReleasesTable(QObject *parent)
    : QObject{parent}
    , m_currentReply(nullptr)
    , m_project{QStringLiteral(QT_STRINGIFY(PR_PROJECT))}
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &ReleasesTable::parsingFinished);

    m_current.version = QVersionNumber::fromString(QT_STRINGIFY(PR_VERSION));
    m_current.date = QDateTime::fromString(QT_STRINGIFY(PR_DATETIME), Qt::ISODate);
    if (QSysInfo::productType() == u"windows"_s)
        m_current.platform =  QSysInfo::productType();
    else if (QSysInfo::productType() == u"macos"_s)
        m_current.platform =  u"mac"_s;
    else if (QSysInfo::kernelType() == u"linux"_s)
        m_current.platform =  QSysInfo::kernelType();
    else
        m_current.platform = u"source"_s;
}

void ReleasesTable::parseFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        m_xml.setDevice(&file);
        parseXml();
        emit parsingFinished();
    }
}

void ReleasesTable::getFromSourceforge()
{
    get(m_urltpl.arg(m_project));
}

void ReleasesTable::consumeData()
{
    int statusCode = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300)
        parseXml();
}

void ReleasesTable::error()
{
    qWarning() << tr("error retrieving RSS feed");
    m_xml.clear();
    m_currentReply->disconnect(this);
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void ReleasesTable::get(const QUrl &url)
{
    if (m_currentReply) {
        m_currentReply->disconnect(this);
        m_currentReply->deleteLater();
    }
    m_currentReply = url.isValid() ? m_manager->get(QNetworkRequest(url)) : nullptr;
    if (m_currentReply) {
        connect(m_currentReply, &QNetworkReply::readyRead, this, &ReleasesTable::consumeData);
        connect(m_currentReply, &QNetworkReply::errorOccurred, this, &ReleasesTable::error);
    }
    m_xml.setDevice(m_currentReply); // Equivalent to clear() if m_currentReply is null.
}

ReleasesList ReleasesTable::filtered() const
{
    ReleasesList result;
    std::copy_if(cbegin(), cend(), std::back_inserter(result), [=](const ReleaseData &data) {
        return data.version >= m_current.version && data.date > m_current.date
               && data.platform == m_current.platform;
    });
    return result;
}

int ReleasesTable::filteredCount() const
{
    return std::count_if(cbegin(), cend(), [=](const ReleaseData &data) {
        return data.version >= m_current.version && data.date > m_current.date
               && data.platform == m_current.platform;
    });
}

void ReleasesTable::parseXml()
{
    while (!m_xml.atEnd()) {
        m_xml.readNext();
        if (m_xml.isStartElement()) {
            if (m_xml.name() == "item"_L1) {
                m_titleString.clear();
                m_pubDateString.clear();
                m_contentString.clear();
                m_sizeString.clear();
                m_linkString.clear();
                m_hashString.clear();
            } else if (m_xml.name() == "content"_L1) {
                m_contentString = m_xml.attributes().value("type"_L1).toString();
                m_sizeString = m_xml.attributes().value("filesize"_L1).toString();
            }
            m_currentTag = m_xml.name().toString();
        } else if (m_xml.isEndElement()) {
            if (m_xml.name() == "item"_L1) {
                ReleaseData item;
                QFileInfo info(m_titleString);
                if (info.suffix() == "exe"_L1) {
                    item.platform = "windows"_L1;
                } else if (info.suffix() == "dmg"_L1) {
                    item.platform = "mac"_L1;
                } else if (info.suffix() == "AppImage"_L1) {
                    item.platform = "linux"_L1;
                } else {
                    item.platform = "source"_L1;
                }
                QRegularExpression rex(m_rextpl.arg(m_project));
                auto match = rex.match(m_titleString);
                if (match.hasMatch()) {
                    item.version = QVersionNumber::fromString(match.captured(1));
                }
                m_pubDateString += 'C';
                item.title = m_titleString;
                QDateTime dt = QDateTime::fromString(m_pubDateString, m_datefmt);
                item.date = dt;
                item.content = m_contentString;
                item.size = m_sizeString.toInt();
                item.link = m_linkString;
                item.hash = m_hashString.toLatin1();
                push_back(std::move(item));
            }
        } else if (m_xml.isCharacters() && !m_xml.isWhitespace() && !m_xml.isComment()) {
            if (m_currentTag == "title"_L1)
                m_titleString += m_xml.text();
            else if (m_currentTag == "pubDate"_L1)
                m_pubDateString += m_xml.text();
            else if (m_currentTag == "link"_L1)
                m_linkString += m_xml.text();
            else if (m_currentTag == "hash"_L1)
                m_hashString += m_xml.text();
        }
    }
    if (m_xml.error() && m_xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
        qWarning() << tr("XML ERROR:") << m_xml.lineNumber() << ": " << m_xml.errorString();
}
