// Copyright (c) 2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RELEASESTABLE_H
#define RELEASESTABLE_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QXmlStreamReader>

#include "releasedata.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace drumstick {
namespace updchk {

using namespace Qt::Literals::StringLiterals;

class DRUMSTICK_UPDCHK_EXPORT ReleasesTable : public QObject, public ReleasesList
{
    Q_OBJECT
public:
    explicit ReleasesTable(const QString projectName,
                           const QString projectVersion,
                           const QString projectDateTime,
                           QObject *parent = nullptr);

    void parseFromFile(const QString &fileName);
    void getFromSourceforge();
    void get(const QUrl &url);

    ReleasesList filtered() const;
    int filteredCount() const;

    const ReleaseData &currentRelease() const { return m_current; }
    const QString &currentProject() const { return m_project; }

public slots:
    void consumeData();
    void error();

signals:
    void parsingFinished();

private:
    void parseXml();

    // Parser state:
    QXmlStreamReader m_xml;
    QString m_currentTag;
    QString m_linkString;
    QString m_titleString;
    QString m_pubDateString;
    QString m_contentString;
    QString m_sizeString;
    QString m_hashString;

    // Network state:
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_currentReply;

    QString m_project;
    ReleaseData m_current;

    static inline const QString m_urltpl{u"https://sourceforge.net/projects/%1/rss?path=/%1"_s};
    static inline const QString m_rextpl{u"^\\/%1\\/(.*)\\/.*$"_s};
    static inline const QString m_datefmt{u"ddd, dd MMM yyyy hh:mm:ss t"_s};
};

} // namespace updchk
} // namespace drumstick

#endif // RELEASESTABLE_H
