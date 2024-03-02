#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QMetaEnum>
#include <QSysInfo>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "bestreleases.h"
#include "mainwindow.h"
#include "releasestable.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    connect(&m_releases, &ReleasesTable::parsingFinished, this, &MainWindow::finishedTable);
    connect(&m_bestReleases, &BestReleases::parsingFinished, this, &MainWindow::finishedBest);

    QWidget *content = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(content);

    QLabel *bestLabel = new QLabel(tr("Best available releases:"), this);
    layout->addWidget(bestLabel);

    m_besTree = new QTreeWidget(this);
    connect(m_besTree, &QTreeWidget::itemActivated, this, &MainWindow::openLink);
    m_besTree->setHeaderLabels(columnNames());
    m_besTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(m_besTree);

    QLabel *tableLabel = new QLabel(tr("All available releases:"), this);
    layout->addWidget(tableLabel);

    m_tree = new QTreeWidget(this);
    connect(m_besTree, &QTreeWidget::itemActivated, this, &MainWindow::openLink);
    m_tree->setHeaderLabels(columnNames());
    m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(m_tree);

    setCentralWidget(content);

    setWindowTitle(tr("Sourceforge File Releases"));
    resize(1440, 600);

    populateData();
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

void MainWindow::finishedTable()
{
    for (const ReleaseData &rel : m_releases.filtered()) {
        addReleaseToTree(rel, m_tree);
    }
}

void MainWindow::finishedBest()
{
    for (const ReleaseData &rel : m_bestReleases.filtered()) {
        addReleaseToTree(rel, m_besTree);
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

void MainWindow::populateData()
{
    QFileInfo fi(QApplication::applicationFilePath());
    m_bestReleases.getFromSourceforge();
    //m_bestReleases.parseFromFile(fi.absolutePath() + "/best_release.json");
    m_releases.getFromSourceforge();
    //m_releases.parseFromFile(fi.absolutePath() + "/rss.xml");
}
