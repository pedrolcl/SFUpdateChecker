#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

#include "bestreleases.h"
#include "releasedata.h"
#include "releasestable.h"

class QTreeWidget;
class QTreeWidgetItem;

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
    ReleasesTable m_releases;
    BestReleases m_bestReleases;

    QTreeWidget *m_tree;
    QTreeWidget *m_besTree;
};

#endif // MAINWINDOW_H
