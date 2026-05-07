#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ListWithWxtremes.hpp"
#include "WarstwaUslug.h"
#include "connectionwindow.h"
#include "netservice.h"

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

    PROP(uint32_t, MainWindow)
        GETTER(uint32_t)
        void set(const uint32_t& value)
        {
            assert(value > 0);
            this->value = value;
        }
    } oknoObserwacji;

public slots:
    void externalUIUpdate();
    void chartsUpdate(UAR::Tick tick, uint32_t czas);
private slots:

    void on_pushButton_startStop_clicked();

    void on_spinBox_wzmocnienie_editingFinished();

    void on_stalaCalkowa_editingFinished();

    void on_stalaRozniczkowa_editingFinished();

    void on_resetPID_clicked();

    void on_szerokoscHisterezy_editingFinished();

    void on_wartoscSterowaniaON_editingFinished();

    void on_radioButton_pid_clicked();

    void on_radioButton_onoff_clicked();

    void on_rozmiarWykresu_editingFinished();

    void on_interwal_editingFinished();

    void on_czasTR_editingFinished();

    void on_amplituda_editingFinished();

    void on_skladowaStala_editingFinished();

    void on_wypelnienie_editingFinished();

    void on_radioButton_prostokatny_clicked();

    void on_radioButton_sinusoidalny_clicked();



    void on_nasycenieMax_editingFinished();

    void on_nasycenieMin_editingFinished();

    void on_pushButton_arx_clicked();

    void on_pushButton_reset_clicked();

    void on_checkBox_ograniczenie_clicked();

    void on_checkBox_nasycenie_clicked();

    void on_actionPolacz_triggered();

    void on_pidWewn_clicked();

    void on_pidZewn_clicked();

private:
    Ui::MainWindow *ui;
    WarstaUslug uslugi;
    ParametryARX* paraARX = nullptr;
    ConnectionWindow *m_connWindow = nullptr;

    void applyNetworkRoleBlocking();

    ListWithExtremes pamiec_wykresow;


};
#endif // MAINWINDOW_H
