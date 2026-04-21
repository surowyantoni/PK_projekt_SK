#include "mainwindow.h"

#include <QApplication>
#include <utils.hpp>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
