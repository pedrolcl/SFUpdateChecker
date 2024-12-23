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

#if defined(DRUMSTICK_STATIC)
#define DRUMSTICK_UPDCHK_EXPORT
#else
#if defined(drumstick_updchk_EXPORTS)
#define DRUMSTICK_UPDCHK_EXPORT Q_DECL_EXPORT
#else
#define DRUMSTICK_UPDCHK_EXPORT Q_DECL_IMPORT
#endif
#endif

namespace drumstick {
namespace updchk {

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

} // namespace updchk
} // namespace drumstick

#endif // RELEASEDATA_H
