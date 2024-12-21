// Copyright (c) 2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QDateTime>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
