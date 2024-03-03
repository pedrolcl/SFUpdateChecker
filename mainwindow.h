// Copyright (c) 2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

#include "releasedata.h"

class QTreeWidget;
class QTreeWidgetItem;
class BestReleases;
class ReleasesTable;

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
    void populateData();
    void addReleaseToTree(const ReleaseData &releaseData, QTreeWidget *tree);

    // members
    ReleasesTable *m_releases;
    BestReleases *m_bestReleases;

    QTreeWidget *m_tree;
    QTreeWidget *m_besTree;
};

#endif // MAINWINDOW_H
