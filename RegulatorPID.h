#pragma once

class RegulatorPID
{
public:
    enum SposobLiczeniaCalki { Zewnetrzne = 0, Wewnetrzne = 1 };

private:
    double wartosc_przed_nasyceniem;
    double sterowanieMax = 5.0;  // górna granica sterowania
    double sterowanieMin = -5.0; // dolna granica sterowania
    double anitWindup = 0.1;       // współczynnik anti-windup
    bool czyWindup = false;

    double m_Uchyb;

    double m_SkladowaProporcjonalna;
    double m_SkladowaCalkowania;
    double m_SkladowaRozniczkowania;
    double m_WartoscSterowania;

    double m_Wzmocnienie;
    double m_StalaCalkowania;
    double m_StalaRozniczkowania;

    double m_UchybPoprzedni;
    double m_SumaUchybuOdPoczatku;

    double sumaUchybowCalkowanieZewnetrzne;
    double sumaUchybowCalkowanieWewnetrzne;

    SposobLiczeniaCalki m_AktualnySposobLiczeniaCalki;  // 0 - zew, 1 - wew
    SposobLiczeniaCalki m_PoprzedniSposobLiczeniaCalki; // 0 - zew, 1 - wew

    void Oblicz_m_SkladowaProporcjonalna();
    void Oblicz_m_SkladowaCalkowania();
    void Oblicz_m_SkladowaRozniczkowania();

public:
    void Zmiana_m_SposobLiczeniaCalki(SposobLiczeniaCalki sposobLiczeniaCalki);
    double Symuluj(double UchybI);
    void set_m_StalaCalkowania(double Stala);
    void Reset();
    RegulatorPID();
    RegulatorPID(double wzmocnienie, double stalaCalkowania, double stalaRozniczkowania);
    RegulatorPID(double Wzmocnienie);
    RegulatorPID(double Wzmocnienie, double StalaC);
    void ResetCalkowania();

    void set_m_SkladowaProporcjonalna(double SkladowaP);
    void set_m_SkladowaCalkowania(double SkladowaC);
    void set_m_SkladowaRozniczkowania(double SkladowaR);
    void set_m_WartoscSterowania(double WartoscS);
    void set_m_Wzmocnienie(double Wzmocnienie);

    void set_m_StalaRozniczkowania(double StalaR);
    void set_m_SumaUchybuOdPoczatku(double SumaUchybuOdPoczatku);
    void set_m_Uchyb(double Uchyb);
    void set_m_UchybPoprzedni(double UchybP);
    void set_m_czyWindup(bool czy);

    void set_m_SposobLiczeniaCalki(SposobLiczeniaCalki sposobLiczeniaCalki);

    double get_m_SkladowaProporcjonalna();
    double get_m_SkladowaCalkowania();
    double get_m_SkladowaRozniczkowania();
    double get_m_WartoscSterowania();
    double get_m_Wzmocnienie();
    double get_m_StalaCalkowania();
    double get_m_StalaRozniczkowania();
    double get_m_SumaUchybuOdPoczatku();
    double get_m_Uchyb();
    double get_m_UchybPoprzedni();
};
