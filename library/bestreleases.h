#ifndef BESTRELEASES_H
#define BESTRELEASES_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>

#include "releasedata.h"

using namespace Qt::Literals::StringLiterals;

class BestReleases : public QObject, public ReleasesList
{
    Q_OBJECT

public:
    explicit BestReleases(QObject *parent = nullptr);

    void parseFromFile(const QString &fileName);
    void getFromSourceforge();
    void get(const QUrl &url);

    ReleasesList filtered() const;

public slots:
    void consumeData();
    void error(QNetworkReply::NetworkError);

signals:
    void parsingFinished();

private:
    void parseOneRelease(const QLatin1StringView key, const QJsonObject obj);
    void parseJson();

    // Parser state:
    QJsonDocument m_json;

    // Network state:
    QNetworkAccessManager m_manager;
    QNetworkReply *m_currentReply;

    QString m_project;
    ReleaseData m_current;

    static inline const QString m_urltpl{u"https://sourceforge.net/projects/%1/best_release.json"_s};
    static inline const QString m_dwntpl{u"https://sourceforge.net/projects/%1/files%2/download"_s};
    static inline const QString m_rextpl{u"^\\/%1\\/(.*)\\/%1.*$"_s};
    static inline const QString m_datefmt{u"yyyy-MM-dd hh:mm:ss"_s};
};

#endif // BESTRELEASES_H
