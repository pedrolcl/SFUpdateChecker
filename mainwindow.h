// Copyright (c) 2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

#include "releasedata.h"

class QLabel;
class QTreeWidget;
class QTreeWidgetItem;

namespace drumstick {
namespace updchk {

class BestReleases;
class ReleasesTable;

} // namespace updchk
} // namespace drumstick

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum Columns { Version, Platform, Title, Date, Content, Size, Link, Hash };
    Q_ENUM(Columns)

    MainWindow(QWidget *parent = nullptr);

public slots:
    void finishedTable();
    void finishedBest();
    void openLink(QTreeWidgetItem *item);

private:
    QStringList columnNames();
    void checkForUpdates();
    void addReleaseToTree(const drumstick::updchk::ReleaseData &releaseData, QTreeWidget *tree);
    void showAvailableUpdates(int rows);
    void showNoUpdatesAvailable();

    // members
    drumstick::updchk::ReleasesTable *m_releases;
    drumstick::updchk::BestReleases *m_bestReleases;

    QTreeWidget *m_tree;
    QTreeWidget *m_besTree;
    QLabel *m_bestLabel;
    QLabel *m_tableLabel;
};

#endif // MAINWINDOW_H
