#ifndef PARAMETRYARX_H
#define PARAMETRYARX_H

#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <vector>

#include "mainwindow.h"

namespace Ui {
class ParametryARX;
}

struct Zakres
{
    double from;
    double to;
};

class ParametryARX : public QDialog
{
    Q_OBJECT

public:
    explicit ParametryARX(MainWindow *parentWindow);

    void addNewFieldVectorA(double value = 0.0);
    void addNewFieldVectorB(double value = 0.0);
    std::vector<double> readAllFieldsVectorA();
    std::vector<double> readAllFieldsVectorB();
    double readOpoznienie();
    double readSzum();
    Zakres readZakresSterowania();
    Zakres readZakresRegulowania();
    bool readCzyOpoznienie();
    ~ParametryARX();
    double readMax();
    double readMin();
    double readRegMax();
    double readRegMin();

private slots:
    void on_addAreaVectorA_clicked();

    void on_buttonBox_accepted();

private:
    Ui::ParametryARX *ui;

    QVBoxLayout *dynamicLayoutVectorA;
    QVBoxLayout *dynamicLayoutVectorB;

    MainWindow *m_parent;
};

#endif // PARAMETRYARX_H
