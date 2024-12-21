// Copyright (c) 2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BESTRELEASES_H
#define BESTRELEASES_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>

#include "releasedata.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace drumstick {
namespace updchk {

using namespace Qt::Literals::StringLiterals;

class DRUMSTICK_UPDCHK_EXPORT BestReleases : public QObject, public ReleasesList
{
    Q_OBJECT

public:
    explicit BestReleases(const QString projectName,
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
    void parseOneRelease(const QLatin1StringView key, const QJsonObject obj);
    void parseJson();

    // Parser state:
    QJsonDocument m_json;

    // Network state:
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_currentReply;

    QString m_project;
    ReleaseData m_current;

    static inline const QString m_urltpl{u"https://sourceforge.net/projects/%1/best_release.json"_s};
    static inline const QString m_dwntpl{u"https://sourceforge.net/projects/%1/files%2/download"_s};
    static inline const QString m_rextpl{u"^\\/%1\\/(.*)\\/%1.*$"_s};
    static inline const QString m_datefmt{u"yyyy-MM-dd hh:mm:ss"_s};
};

} // namespace updchk
} // namespace drumstick

#endif // BESTRELEASES_H
