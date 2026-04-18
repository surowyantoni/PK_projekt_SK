#include "WarstwaUslug.h"
#include <QApplication>
#include <QDebug>

void WarstaUslug::set_wspolczynniki(vector<double> zadaneWspolczynnikiA,
                                    vector<double> zadaneWspolczynnikiB)
{
    m_wspolczynnikiA = zadaneWspolczynnikiA;
    m_wspolczynnikiB = zadaneWspolczynnikiB;
}

void WarstaUslug::set_m_opoznienieTransportowe(double opoznienie)
{
    m_opoznienieTransportowe = opoznienie;
}

void WarstaUslug::set_m_wartoscZaklocen(double zaklocenie)
{
    m_wartoscZaklocen = zaklocenie;
}

void WarstaUslug::set_m_ograniczenieMaxZad(double ograniczenieMax)
{
    m_ograniczenieMaxZad = ograniczenieMax;
}

void WarstaUslug::set_m_ograniczenieMinZad(double ograniczenieMin)
{
    m_ograniczenieMinZad = ograniczenieMin;
}

void WarstaUslug::set_m_ograniczenieMaxReg(double ograniczenieMax)
{
    m_ograniczenieMaxReg = ograniczenieMax;
}

void WarstaUslug::set_m_ograniczenieMinReg(double ograniczenieMin)
{
    m_ograniczenieMinReg = ograniczenieMin;
}

void WarstaUslug::set_m_czyOgraniczenie(bool czy)
{
    m_czyOgraniczenie = czy;
}

void WarstaUslug::set_m_arx()
{
    if (m_wspolczynnikiA != m_arx.getA() || m_wspolczynnikiB != m_arx.getB()) {
        m_arx.setA(m_wspolczynnikiA);
        m_arx.setB(m_wspolczynnikiB);
    }

    m_arx.setZaklocenia(m_wartoscZaklocen);
    m_arx.setOpoznienie(m_opoznienieTransportowe);

    m_arx.setMaxWejscia(m_ograniczenieMaxZad);
    m_arx.setMinWejscia(m_ograniczenieMinZad);
    m_arx.setMaxWyjscia(m_ograniczenieMaxReg);
    m_arx.setMinWyjscia(m_ograniczenieMinReg);
    m_arx.setOgraniczenie(m_czyOgraniczenie);
}

void WarstaUslug::set_m_wzmocnienie(double wzmocnienie)
{
    m_wzmocnienie = wzmocnienie;
}

void WarstaUslug::set_m_stalaCalkowania(double stalaCalkowania)
{
    m_stalaCalkowania = stalaCalkowania;
}

void WarstaUslug::set_m_stalaRozniczkowania(double stalaRozniczkowania)
{
    m_stalaRozniczkowania = stalaRozniczkowania;
}

void WarstaUslug::set_m_sposobLiczeniaCalki(RegulatorPID::SposobLiczeniaCalki sposobLiczeniaCalki)
{
    m_sposobLiczeniaCalki = sposobLiczeniaCalki;
}

void WarstaUslug::reset_m_pid()
{
    m_pid.Reset();
}
void WarstaUslug::resetCalkowania()
{
    m_pid.ResetCalkowania();
}

void WarstaUslug::set_m_pid()
{
    m_pid.set_m_StalaCalkowania(m_stalaCalkowania);
    m_pid.set_m_StalaRozniczkowania(m_stalaRozniczkowania);
    m_pid.set_m_Wzmocnienie(m_wzmocnienie);
    m_pid.Zmiana_m_SposobLiczeniaCalki(m_sposobLiczeniaCalki);
}

void WarstaUslug::set_m_Tt(int Tt)
{
    m_Tt = Tt;
}

void WarstaUslug::set_m_Tr(int Tr)
{
    m_Tr = Tr;
}

void WarstaUslug::set_m_A(int A)
{
    m_A = A;
}

void WarstaUslug::set_m_S(int S)
{
    m_S = S;
}

void WarstaUslug::set_m_c(int c)
{
    m_c = c;
}

void WarstaUslug::set_m_p(double p)
{
    m_p = p;
}

void WarstaUslug::set_wartoscZadana() {}

void WarstaUslug::set_m_h(double h)
{
    m_h = h;
}

void WarstaUslug::set_m_uON(double uON)
{
    m_uON = uON;
}

void WarstaUslug::set_m_stan(bool stan)
{
    m_stan = stan;
}

void WarstaUslug::set_m_onOff()
{
    m_onOff = RegulatorOnOff(m_uON, m_h);
    m_onOff.reset();
}

void WarstaUslug::setPID()
{
    m_uar.setPID(m_pid);
}

void WarstaUslug::setOnOff()
{
    m_uar.setOnOff(m_onOff);
}

void WarstaUslug::set_czasRzeczywisty(int Trr)
{
    m_Tr = Trr;
    m_genW.setCzasRzecz(Trr);
}

void WarstaUslug::set_czasTaktowania()
{
    m_genW.setCzasTakt(freq);
}

void WarstaUslug::set_Amplituda(int A)
{
    m_A = A;
    m_genW.setAmplituda(A);
}

void WarstaUslug::set_stalaCzasowa(int S)
{
    m_S = S;
    m_genW.setSkladoaStala(S);
}

void WarstaUslug::set_choice(bool wybor)
{
    m_c = wybor;
    m_genW.setTyp((GenW::Typ)wybor);
}

void WarstaUslug::set_wypelnienie(double P)
{
    m_p = P;
    m_genW.setWypelnienie(P);
}

WarstaUslug::WarstaUslug()
    : m_arx()
    , m_pid()
    , m_onOff()
    , m_uar(m_arx, m_pid)
    , m_genW()
{
    //kontrola symulacji
    //parent=p;
    czas = 0.0;
    freq = 200;
    QObject::connect(&timer, &QTimer::timeout, this, &WarstaUslug::symulacja);
}
WarstaUslug::~WarstaUslug()
{
    timer.stop();
}
double WarstaUslug::getUchyb()
{
    if (m_uar.get_choice()) {
        return m_onOff.getWartoscSterowania() - getWartoscRegulowana();
    } else {
        return m_pid.get_m_Uchyb();
    }
}

double WarstaUslug::getWartoscRegulowana()
{
    return m_arx.getWartoscY();
}

double WarstaUslug::symuluj(double wejscie)
{
    return m_uar.Symuluj(wejscie);
}

double WarstaUslug::getWartoscSterowania()
{
    if (!m_uar.get_choice()) {
        return m_pid.get_m_WartoscSterowania();
    } else {
        return m_onOff.getWartoscSterowania();
    }
}

double WarstaUslug::getSkladowaC()
{
    return m_pid.get_m_SkladowaCalkowania();
}
double WarstaUslug::getSkladowaR()
{
    return m_pid.get_m_SkladowaRozniczkowania();
}
double WarstaUslug::getSkladowaP()
{
    return m_pid.get_m_SkladowaProporcjonalna();
}

//kontrola symulacji

void WarstaUslug::rozpocznij()
{
    if (!isActive) {
        //timer.setInterval([&]() { symulacja(); }, freq);
        timer.setInterval(freq);
        timer.start();
        isActive = true;
    } else {
        timer.stop();
        isActive = false;
    }
}

void WarstaUslug::symulacja()
{
    oblicz_WartoscZadana();
    wartoscRegulowana = symuluj(wartoscZadana);
    if (m_onDataReady) {
        m_onDataReady();
    }
    qDebug() << czas;
    czas += static_cast<double>(freq) / 1000;
}

double WarstaUslug::getWartoscZadana()
{
    return wartoscZadana;
}

void WarstaUslug::setFreq(int f)
{
    freq = f;
    timer.setInterval(freq);
    set_czasTaktowania();
}

double WarstaUslug::getCzas()
{
    return czas;
}

double WarstaUslug::getWartoscR()
{
    return wartoscRegulowana;
}

void WarstaUslug::resetCzas()
{
    czas = 0;
}

void WarstaUslug::oblicz_WartoscZadana()
{
    wartoscZadana = m_genW.generuj();
}

vector<double> WarstaUslug::get_m_WspolczynnikiA()
{
    return m_wspolczynnikiA;
}
vector<double> WarstaUslug::get_m_WspolczynnikiB()
{
    return m_wspolczynnikiB;
}
double WarstaUslug::get_m_OpoznienieTransportowe()
{
    return m_opoznienieTransportowe;
}
double WarstaUslug::get_m_OgraniczenieMax()
{
    return m_ograniczenieMaxZad;
}
double WarstaUslug::get_m_OgraniczenieMin()
{
    return m_ograniczenieMinZad;
}
double WarstaUslug::get_m_Zaklucenie()
{
    return m_wartoscZaklocen;
}
double WarstaUslug::get_m_ograniczenieRegulowaniaMax()
{
    return m_ograniczenieMaxReg;
}
double WarstaUslug::get_m_ograniczenieRegulowaniaMin()
{
    return m_ograniczenieMinReg;
}
bool WarstaUslug::get_m_czyOgraniczenie()
{
    return m_czyOgraniczenie;
}

double WarstaUslug::get_m_wzmocnienie()
{
    return m_wzmocnienie;
}
double WarstaUslug::get_m_stalaCalkowania()
{
    return m_stalaCalkowania;
}
double WarstaUslug::get_m_stalaRozniczkowania()
{
    return m_stalaRozniczkowania;
}
RegulatorPID::SposobLiczeniaCalki WarstaUslug::get_m_sposobLiczeniaCalki()
{
    return m_sposobLiczeniaCalki;
}

//OnOff
double WarstaUslug::get_m_h()
{
    return m_h;
}
double WarstaUslug::get_m_uON()
{
    return m_uON;
}

void WarstaUslug::set_wybranyRegulator(WybranyRegulator reg)
{
    m_regulator = reg;
}
WybranyRegulator WarstaUslug::get_wybranyRegulator()
{
    return m_regulator;
}
int WarstaUslug::get_m_Tr()
{
    return m_Tr;
}
int WarstaUslug::get_m_A()
{
    return m_A;
}
int WarstaUslug::get_m_S()
{
    return m_S;
}
int WarstaUslug::get_m_c()
{
    return m_c;
}
double WarstaUslug::get_m_p()
{
    return m_p;
}

void WarstaUslug::set_m_windup(bool czy)
{
    m_pid.set_m_czyWindup(czy);
}

void WarstaUslug::set_m_nasycenie(bool czy)
{
    m_uar.setCzynasycenie(czy);
}
void WarstaUslug::set_m_nasycenieMax(double max)
{
    m_uar.setNasycenieMax(max);
}
void WarstaUslug::set_m_nasycenieMin(double min)
{
    m_uar.setNasycenieMin(min);
}
