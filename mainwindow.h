#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ListWithWxtremes.hpp"
#include "WarstwaUslug.h"
#include "connectionwindow.h"
#include "netservice.h"
#include "parametryarxwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    virtual void resizeEvent(QResizeEvent * event) override;

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

    void on_radioButton_pid_clicked();

    void on_radioButton_onoff_clicked();

    void on_radioButton_prostokatny_clicked();

    void on_radioButton_sinusoidalny_clicked();

    void on_pushButton_arx_clicked();

    void on_pushButton_reset_clicked();

    void on_checkBox_ograniczenie_clicked();

    void on_checkBox_nasycenie_clicked();

    void on_actionPolacz_triggered();

    void on_actionRysuj_linie_czasu_triggered();

    void on_horizontalSlider_wypelnienie_valueChanged(int value);

    void on_spinBox_stalaCalkowa_editingFinished();

    void on_spinBox_stalaRozniczkowa_editingFinished();

    void on_pushButton_resetPID_clicked();

    void on_radioButton_pidWewn_clicked();

    void on_radioButton_pidZewn_clicked();

    void on_spinBox_szerokoscHisterezy_editingFinished();

    void on_spinBox_wartoscSterowaniaON_editingFinished();

    void on_spinBox_rozmiarWykresu_editingFinished();

    void on_spinBox_interwal_editingFinished();

    void on_spinBox_okres_editingFinished();

    void on_spinBox_amplituda_editingFinished();

    void on_spinBox_skladowaStala_editingFinished();

    void on_spinBox_wypelnienie_editingFinished();

    void on_spinBox_sterowanieMin_editingFinished();

    void on_spinBox_sterowanieMax_editingFinished();

    void on_actionOtworz_triggered();

    void on_actionZapisz_triggered();

private:
    Ui::MainWindow *ui;
    WarstaUslug uslugi;
    ParametryARXWindow parameters_arx_window;
    ConnectionWindow connection_window;
    ListWithExtremes pamiec_wykresow;
};
#endif // MAINWINDOW_H
