// Copyright (c) 2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RELEASEDATA_H
#define RELEASEDATA_H

#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QUrl>
#include <QVersionNumber>

struct ReleaseData
{
    QVersionNumber version;
    QString platform;
    QDateTime date;

    QString title;
    QString content;
    qsizetype size;
    QUrl link;
    QByteArray hash;
};

typedef QList<ReleaseData> ReleasesList;

#endif // RELEASEDATA_H
