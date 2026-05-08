#ifndef PARAMETRYARXWINDOW_H
#define PARAMETRYARXWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <vector>
#include "WarstwaUslug.h"


namespace Ui {
class ParametryARXWindow;
}

struct Zakres
{
    double from;
    double to;
};

class ParametryARXWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ParametryARXWindow(WarstaUslug& uslugi);

    void addNewFieldVectorA(double value = 0.0);
    void addNewFieldVectorB(double value = 0.0);
    std::vector<double> readAllFieldsVectorA();
    std::vector<double> readAllFieldsVectorB();
    double readOpoznienie();
    double readSzum();
    Zakres readZakresSterowania();
    Zakres readZakresRegulowania();
    bool readCzyOpoznienie();
    ~ParametryARXWindow();
    double readMax();
    double readMin();
    double readRegMax();
    double readRegMin();

    void refreshFromService();

private slots:
    void on_addAreaVectorA_clicked();

    void on_buttonBox_accepted();

private:
    Ui::ParametryARXWindow *ui;

    QVBoxLayout *dynamicLayoutVectorA;
    QVBoxLayout *dynamicLayoutVectorB;

    WarstaUslug& uslugi;
};

#endif // PARAMETRYARXWINDOW_H
