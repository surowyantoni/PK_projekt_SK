#include "mainwindow.h"

#include <QApplication>
#include "DEFINITIONS.hpp"

#ifdef MAIN

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    return a.exec();
}

#endif

