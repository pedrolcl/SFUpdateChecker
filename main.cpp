#include <QApplication>
#include <QDateTime>
#include <QDebug>

#include "mainwindow.h"
#include "releasedata.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
