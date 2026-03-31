#pragma once
#include "ARXgenerate.h"
#include "GenW.h"
#include "RegulatorOnOff.h"
#include "RegulatorPID.h"
#include "UAR.h"

//#include "mainwindow.h"
#include <QObject>
#include <QTimer>
#include <functional>
enum WybranyRegulator { PID = 0, ONOFF = 1, nic = 2 };

class WarstaUslug : public QObject
{
    Q_OBJECT
private:
    ARXgenerate m_arx;
    RegulatorPID m_pid;
    RegulatorOnOff m_onOff;
    UAR m_uar;
    GenW m_genW;
    //arx
    vector<double> m_wspolczynnikiA;
    vector<double> m_wspolczynnikiB;
    int m_opoznienieTransportowe;
    double m_wartoscZaklocen;

    double m_ograniczenieMaxZad = 10;
    double m_ograniczenieMinZad = -10;
    double m_ograniczenieMaxReg = 10;
    double m_ograniczenieMinReg = -10;
    bool m_czyOgraniczenie = true;
    //pid
    double m_wzmocnienie;
    double m_stalaCalkowania;
    double m_stalaRozniczkowania;
    RegulatorPID::SposobLiczeniaCalki m_sposobLiczeniaCalki;
    WybranyRegulator m_regulator;
    //wartosc zadana
    int m_Tt; //Okres cz�stkowy
    int m_Tr; //Czas rzeczywisty
    int m_A;  //amplituda
    int m_S;  //Sta�a sk�adowa
    bool m_c; //wyb�r typu sygna�u
    double m_p;
    //on/off
    double m_h;   //szeroko�� histerezy
    double m_uON; //warto�� sterowania w stanie ON
    bool m_stan;  //aktualny stan regulatora: true = ON, false = OFF
    double m_e;   //uchyb dla on/off

    //kontrola symulacji
    bool isActive = false;
    double wartoscRegulowana = 0;
    double wartoscZadana = 0;
    double czas = 0.0;
    int freq = 200;
    //Timer timer;
    QTimer timer;
    // parent;
    std::function<void()> m_onDataReady;

    //genW
    int Tr, A, S;
    bool c;
    double p;

public:
    WarstaUslug();
    ~WarstaUslug();
    //konfiguracja arx
    void set_wspolczynniki(vector<double> zadaneWspolczynnikiA, vector<double> zadaneWspolczynnikiB);
    void set_m_opoznienieTransportowe(double opoznienie);
    void set_m_wartoscZaklocen(double zaklocenie);

    void set_m_ograniczenieMaxZad(double ograniczenieMax);
    void set_m_ograniczenieMinZad(double ograniczenieMin);
    void set_m_ograniczenieMaxReg(double ograniczenieMax);
    void set_m_ograniczenieMinReg(double ograniczenieMin);
    void set_m_czyOgraniczenie(bool czy);

    void set_m_arx();
    //konfiguracja pid
    void set_m_wzmocnienie(double wzmocnienie);
    void set_m_stalaCalkowania(double stalaCalkowania);
    void set_m_stalaRozniczkowania(double stalaRozniczkowania);
    void set_m_sposobLiczeniaCalki(RegulatorPID::SposobLiczeniaCalki sposobLiczeniaCalki);
    void reset_m_pid();
    void resetCalkowania();
    void set_m_pid();
    //konfiguracja wartosci zadanej
    void set_m_Tt(int Tt);
    void set_m_Tr(int Trr);
    void set_m_A(int A);
    void set_m_S(int S);
    void set_m_c(int c);
    void set_m_p(double p);
    void set_wartoscZadana();
    //konfiguracja on/off
    void set_m_h(double h);
    void set_m_uON(double uON);
    void set_m_stan(bool stan);
    void set_m_onOff();

    //konfiguracja UAR
    void setPID();
    void setOnOff();

    //filtry
    void set_m_nasycenie(bool czy);
    void set_m_windup(bool czy);
    void set_m_nasycenieMax(double max);
    void set_m_nasycenieMin(double min);

    //Dane z UAR do GUI
    double getUchyb();
    double getWartoscRegulowana();
    double symuluj(double wejscie);
    double getWartoscSterowania();
    double getSkladowaC();
    double getSkladowaR();
    double getSkladowaP();

    //Get'y do Zapisu
    //gety ARX
    vector<double> get_m_WspolczynnikiA();
    vector<double> get_m_WspolczynnikiB();
    double get_m_OpoznienieTransportowe();
    double get_m_OgraniczenieMax();
    double get_m_OgraniczenieMin();
    double get_m_ograniczenieRegulowaniaMax();
    double get_m_ograniczenieRegulowaniaMin();
    double get_m_Zaklucenie();
    bool get_m_czyOgraniczenie();
    //gety Ustawionego Regulatora

    //PID
    double get_m_wzmocnienie();
    double get_m_stalaCalkowania();
    double get_m_stalaRozniczkowania();
    RegulatorPID::SposobLiczeniaCalki get_m_sposobLiczeniaCalki();
    void set_wybranyRegulator(WybranyRegulator reg);
    WybranyRegulator get_wybranyRegulator();
    //OnOff
    double get_m_h();   //szerokosc histerezy
    double get_m_uON(); //wartosc sterowania w stanie ON
    //gety generatora
    int get_m_Tr();
    int get_m_A();
    int get_m_S();
    int get_m_c();
    double get_m_p();

    //kontrola symulacji
    void rozpocznij();
    void symulacja();
    void oblicz_WartoscZadana();
    double getWartoscZadana();
    void setFreq(int f);
    double getCzas();
    double getWartoscR();
    void setOnDataReady(std::function<void()> callback) { m_onDataReady = callback; }
    void resetCzas();

    //GenW
    void set_czasRzeczywisty(int Tr);
    void set_czasTaktowania();
    void set_Amplituda(int A);
    void set_stalaCzasowa(int S);
    void set_choice(bool wybor);
    void set_wypelnienie(double P);
};
