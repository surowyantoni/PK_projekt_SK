#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    ARXgenerate test;

    QJsonDocument doc;
    doc.setObject(test.toJSON());
    qDebug() << doc.toJson();

    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
