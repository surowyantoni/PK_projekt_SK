#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QtCharts>
#include <QtCore>
#include <QtGui>
//#include <QTimer>
#include "connectionwindow.h"
#include "netservice.h"
#include "qcustomplot.h"

#include "WarstwaUslug.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    double readWzmocnienie();
    double readStalaCalkowania();
    double readStalaRozniczkowania();
    double readSzerokoscHisterezy();
    double readWartoscSterowaniaOn();
    RegulatorPID::SposobLiczeniaCalki readSposobLiczeniaPID();
    int readCzasRzeczywisty();
    int readAmplituda();
    int readSkladowaStala();
    double readWypelnienie();
    int readTypGeneratora();

    int readWielkoscWykresu();
    bool readPIDCheckBox();
    bool readONOFFCheckBox();
    void wczytajWartosciZeSciezki(QString sciezka);
    void readNasycenieMax();
    void readNasycenieMin();
    void czyWindup();
    void czyNasycenie();

    ~MainWindow();

public slots:
    void onSampleReceived(double u, double y, quint32 k);
    void onPidUpdated(double p, double i, double d, int mode, double min, double max);
    void onGenUpdated(int type, double amp, double per, double off, double duty);
    void onArxUpdated(const QVector<double>& A, const QVector<double>& B, int k, double sigma, double minU, double maxU, double minY, double maxY);

    // Metoda sterująca blokadami
    void applyNetworkRole(bool isRegulator);

private slots:
    void on_ZapiszRegulatory_clicked();
    void on_StartStop_clicked();
    void updateWykres();
    void on_ustawWartoscZ_clicked();
    void on_ustawWykres_clicked();
    void on_InterfejsARX_clicked();
    void on_pushButton_clicked();
    void on_Reset_clicked();
    void on_ustawGenerator_clicked();

    void on_ustawFiltr_clicked();

    void on_resetPID_clicked();

    void on_actionZapisz_triggered();

    void on_actionOtworz_triggered();

    void on_actionPolacz_triggered();

private:
    Ui::MainWindow *ui;
    WarstaUslug m_uslugi;
    ConnectionWindow *connWindow = nullptr;
    NetService *service = nullptr;
    QCPGraph *graph1;
    QCPGraph *graph2;
    QCPGraph *graph3;
    QCPGraph *graph4;
    QCPGraph *graph5;
    double wartoscZ;
    double time = 0;
    //QTimer * timer;
    friend class ParametryARX;

    bool m_isRegulator = false;
    quint32 m_sampleK = 0;

    //double freq=200; częstotliwość odświerzania
    int range = 10; //szerokość wykresu
    double up1 = 1, up2 = 1, up3 = 1, up4 = 1;
    double lp1 = -1, lp2 = -1, lp3 = -1, lp4 = -1;
    int czasRzeczywisty = 0, amplituda = 0, skladowaStala = 0, typGeneratora = 0;
    double wypelnienie = 0.0;
};
#endif // MAINWINDOW_H
