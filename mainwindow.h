#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "WarstwaUslug.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class ParametryARX;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void externalUIUpdate();
    void chartsUpdate(UAR::Tick tick, uint32_t czas);
private slots:

    void on_pushButton_startStop_clicked();

private:
    Ui::MainWindow *ui;
    WarstaUslug uslugi;
    // QCPGraph *graph1;
    // QCPGraph *graph2;
    // QCPGraph *graph3;
    // QCPGraph *graph4;
    // QCPGraph *graph5;
    PROPERTY(uint32_t , SzerokoscOknaObserwacjiWykresowWMilisekundach)
        //TODO ograniczenia
    } oknoObserwacji;

};
#endif // MAINWINDOW_H
