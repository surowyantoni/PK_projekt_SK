#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , uslugi(WarstaUslug())
    , oknoObserwacji(10.0 * 1000)
{
    ui->setupUi(this);
    externalUIUpdate();
}
void MainWindow::externalUIUpdate()
{

}
void MainWindow::chartsUpdate(UAR::Tick tick, uint32_t czas)
{
    qDebug() << "CZas:" << czas << "TICK:" << tick.wartoscZadana;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_startStop_clicked()
{
    uslugi.dziala = true;
}

