// Copyright (c) 2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QMetaEnum>
#include <QSysInfo>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "bestreleases.h"
#include "mainwindow.h"
#include "releasestable.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_releases = new ReleasesTable(this);
    connect(m_releases, &ReleasesTable::parsingFinished, this, &MainWindow::finishedTable);

    m_bestReleases = new BestReleases(this);
    connect(m_bestReleases, &BestReleases::parsingFinished, this, &MainWindow::finishedBest);

    QWidget *content = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(content);

    m_bestLabel = new QLabel(this);
    layout->addWidget(m_bestLabel);

    m_besTree = new QTreeWidget(this);
    connect(m_besTree, &QTreeWidget::itemActivated, this, &MainWindow::openLink);
    m_besTree->setHeaderLabels(columnNames());
    m_besTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(m_besTree);

    m_tableLabel = new QLabel(this);
    layout->addWidget(m_tableLabel);

    m_tree = new QTreeWidget(this);
    connect(m_tree, &QTreeWidget::itemActivated, this, &MainWindow::openLink);
    m_tree->setHeaderLabels(columnNames());
    m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(m_tree);

    setCentralWidget(content);

    setWindowTitle(tr("Sourceforge File Releases"));
    resize(1440, 600);

    QTimer::singleShot(1000, this, &MainWindow::checkForUpdates);
}

void MainWindow::openLink(QTreeWidgetItem *item)
{
    QDesktopServices::openUrl(QUrl(item->text(Columns::Link)));
}

void MainWindow::addReleaseToTree(const ReleaseData &releaseData, QTreeWidget *tree)
{
    //qDebug() << Q_FUNC_INFO;
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(Columns::Version, releaseData.version.toString());
    item->setText(Columns::Platform, releaseData.platform);
    item->setText(Columns::Title, releaseData.title);
    item->setText(Columns::Date, releaseData.date.toString(Qt::ISODate));
    item->setText(Columns::Content, releaseData.content);
    item->setText(Columns::Size, QString::number(releaseData.size));
    item->setText(Columns::Link, releaseData.link.toDisplayString());
    item->setText(Columns::Hash, releaseData.hash);
    tree->addTopLevelItem(item);
}

void MainWindow::showAvailableUpdates(int rows)
{
    QString title{tr("An Update is Available")};
    QString text{
        tr("You have %n %1 update(s) available for %2. Click one of the following link(s) to "
           "download it.<br/>",
           nullptr,
           rows)
            .arg(m_releases->currentProject(), QSysInfo::prettyProductName())};
    for (const ReleaseData &rel : m_releases->filtered()) {
        text += "<a href='" + rel.link.toString() + "'>" + rel.title + "</a><br/>";
    }
    if (m_releases->currentRelease().platform == "linux") {
        text += tr("You may also want to try a <a "
                   "href='https://flathub.org/apps/search?q=%1'>Flatpak</a>, instead<br/>")
                    .arg(m_releases->currentProject());
    }
    QMessageBox::information(this, title, text);
}

void MainWindow::showNoUpdatesAvailable()
{
    QString title{tr("No Updates Available")};
    QString text{tr("You are already running the latest %1 release available for %2.")
                     .arg(m_releases->currentProject(), QSysInfo::prettyProductName())};
    QMessageBox::information(this, title, text);
}

void MainWindow::finishedTable()
{
    int rows = m_releases->filteredCount();
    m_tableLabel->setText(tr("All Available Releases: %n row(s)", nullptr, rows));
    if (rows > 0) {
        for (const ReleaseData &rel : m_releases->filtered()) {
            addReleaseToTree(rel, m_tree);
        }
        showAvailableUpdates(rows);
    } else {
        showNoUpdatesAvailable();
    }
}

void MainWindow::finishedBest()
{
    int rows = m_bestReleases->filteredCount();
    m_bestLabel->setText(tr("Best Available Releases: %n row(s)", nullptr, rows));
    if (rows > 0) {
        for (const ReleaseData &rel : m_bestReleases->filtered()) {
            addReleaseToTree(rel, m_besTree);
        }
    }
}

QStringList MainWindow::columnNames()
{
    QStringList res;
    auto me = QMetaEnum::fromType<Columns>();
    for (int k = 0; k < me.keyCount(); ++k) {
        res += tr(me.key(k));
    }
    return res;
}

void MainWindow::checkForUpdates()
{
#ifdef DEBUG
    QFileInfo fi(QApplication::applicationFilePath());
    m_bestReleases->parseFromFile(fi.absolutePath() + "/best_release.json");
    m_releases->parseFromFile(fi.absolutePath() + "/rss.xml");
#else
    m_bestReleases->getFromSourceforge();
    m_releases->getFromSourceforge();
#endif
}
