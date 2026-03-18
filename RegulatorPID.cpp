#include "RegulatorPID.h"
#include "QDebug"

void RegulatorPID::set_m_SkladowaProporcjonalna(double SkladowaP)
{
    m_SkladowaProporcjonalna = SkladowaP;
}

void RegulatorPID::set_m_SkladowaCalkowania(double SkladowaC)
{
    m_SkladowaCalkowania = SkladowaC;
}

void RegulatorPID::set_m_SkladowaRozniczkowania(double SkladowaR)
{
    m_SkladowaRozniczkowania = SkladowaR;
}

void RegulatorPID::set_m_WartoscSterowania(double WartoscS)
{
    m_WartoscSterowania = WartoscS;
}

void RegulatorPID::set_m_Wzmocnienie(double Wzmocnienie)
{
    m_Wzmocnienie = Wzmocnienie;
}

void RegulatorPID::set_m_StalaCalkowania(double StalaC)
{
    m_StalaCalkowania = StalaC;
}

void RegulatorPID::Reset()
{
    set_m_SkladowaCalkowania(0);
    set_m_SkladowaRozniczkowania(0);
}

void RegulatorPID::ResetCalkowania()
{
    set_m_SkladowaCalkowania(0);
    set_m_SumaUchybuOdPoczatku(0);
}

void RegulatorPID::set_m_StalaRozniczkowania(double StalaR)
{
    m_StalaRozniczkowania = StalaR;
}

void RegulatorPID::set_m_SumaUchybuOdPoczatku(double SumaUchybuOdPoczatku)
{
    m_SumaUchybuOdPoczatku = SumaUchybuOdPoczatku;
}

void RegulatorPID::set_m_Uchyb(double wartosc)
{
    m_Uchyb = wartosc;
}

void RegulatorPID::set_m_UchybPoprzedni(double UchybP)
{
    m_UchybPoprzedni = UchybP;
}

void RegulatorPID::set_m_SposobLiczeniaCalki(SposobLiczeniaCalki sposobLiczeniaCalki)
{
    m_AktualnySposobLiczeniaCalki = sposobLiczeniaCalki;
}

double RegulatorPID::get_m_SkladowaProporcjonalna()
{
    return m_SkladowaProporcjonalna;
}

double RegulatorPID::get_m_SkladowaCalkowania()
{
    return m_SkladowaCalkowania;
}

double RegulatorPID::get_m_SkladowaRozniczkowania()
{
    return m_SkladowaRozniczkowania;
}

double RegulatorPID::get_m_WartoscSterowania()
{
    return m_WartoscSterowania;
}

double RegulatorPID::get_m_Wzmocnienie()
{
    return m_Wzmocnienie;
}

double RegulatorPID::get_m_StalaCalkowania()
{
    return m_StalaCalkowania;
}

double RegulatorPID::get_m_StalaRozniczkowania()
{
    return m_StalaRozniczkowania;
}

double RegulatorPID::get_m_SumaUchybuOdPoczatku()
{
    return m_SumaUchybuOdPoczatku;
}

double RegulatorPID::get_m_Uchyb()
{
    return m_Uchyb;
}

double RegulatorPID::get_m_UchybPoprzedni()
{
    return m_UchybPoprzedni;
}

void RegulatorPID::Oblicz_m_SkladowaProporcjonalna() // obliczanie skladowej proporcjonalnej
{
    set_m_SkladowaProporcjonalna(get_m_Wzmocnienie() * get_m_Uchyb());
}

void RegulatorPID::Oblicz_m_SkladowaCalkowania()
{
    {
        // Obliczamy standardowy przyrost całki
        double przyrost = 0;
        if (m_AktualnySposobLiczeniaCalki == SposobLiczeniaCalki::Zewnetrzne) {
            przyrost = get_m_Uchyb();
        } else {
            przyrost = get_m_Uchyb() / m_StalaCalkowania;
        }

        // Anti-Windup: Back-calculation
        if (m_czyWindup) {
            // wartosc_pNas musi być przekazana lub obliczona wcześniej (u_ideal - u_sat)
            double bladNasycenia = get_m_WartoscSterowania() - wartosc_pNas;
            if (m_AktualnySposobLiczeniaCalki == SposobLiczeniaCalki::Zewnetrzne) {
                przyrost += (m_Kaw * bladNasycenia * m_StalaCalkowania);
            } else {
                przyrost += (m_Kaw * bladNasycenia);
            }
        }

        m_SumaUchybuOdPoczatku += przyrost;

        // Ustawienie składowej końcowej
        if (m_AktualnySposobLiczeniaCalki == SposobLiczeniaCalki::Zewnetrzne) {
            set_m_SkladowaCalkowania(m_SumaUchybuOdPoczatku / m_StalaCalkowania);
        } else {
            set_m_SkladowaCalkowania(m_SumaUchybuOdPoczatku);
        }
    }
}

void RegulatorPID::Oblicz_m_SkladowaRozniczkowania() //obliczanie skladowej rozniczkowania
{
    set_m_SkladowaRozniczkowania(get_m_StalaRozniczkowania()
                                 * (get_m_Uchyb() - get_m_UchybPoprzedni()));
}

void RegulatorPID::Zmiana_m_SposobLiczeniaCalki(
    SposobLiczeniaCalki
        sposobLiczeniaCalki) //wykrycie zmiany sposobu liczenia calki i przeliczenie sumy uchybu
{
    SposobLiczeniaCalki m_PoprzedniSposobLiczeniaCalki = m_AktualnySposobLiczeniaCalki;
    if (sposobLiczeniaCalki == m_PoprzedniSposobLiczeniaCalki) {
        qDebug() << "takie same";
        set_m_SposobLiczeniaCalki(sposobLiczeniaCalki);
        return; //jezeli nie ma zmiany sposobu liczenia calki to wychodzimy z funkcji
    }
    if (m_StalaCalkowania == 0.0) {
        qDebug() << "stala = 0";
        m_AktualnySposobLiczeniaCalki
            = sposobLiczeniaCalki; //jezeli stala calkowania = 0 to mozemy poprostu podmienic bez obawy o zmiany wartosci
        return;
    }

    if (sposobLiczeniaCalki == SposobLiczeniaCalki::Wewnetrzne
        && m_PoprzedniSposobLiczeniaCalki == SposobLiczeniaCalki::Zewnetrzne) {
        qDebug() << "zwe na wew";
        m_SumaUchybuOdPoczatku
            = m_SumaUchybuOdPoczatku
              / m_StalaCalkowania; //przeliczenie sumy uchybu przy zmianie sposobu liczenia calki, w sposob gdy z Zewnetrznego na Wewnetrzny
    } else if (sposobLiczeniaCalki == SposobLiczeniaCalki::Zewnetrzne
               && m_PoprzedniSposobLiczeniaCalki == SposobLiczeniaCalki::Wewnetrzne) {
        qDebug() << "wew na zew";
        m_SumaUchybuOdPoczatku
            = m_SumaUchybuOdPoczatku
              * m_StalaCalkowania; //przeliczenie sumy uchybu przy zmianie sposobu liczenia calki, gdy z Wewnetrznego na Zewnetrzny
    }
    qDebug() << "przeszlo";
    set_m_SposobLiczeniaCalki(sposobLiczeniaCalki);
}

double RegulatorPID::Symuluj(double UchybI)
{
    set_m_Uchyb(UchybI);

    Oblicz_m_SkladowaProporcjonalna();
    Oblicz_m_SkladowaRozniczkowania();

    if (get_m_StalaCalkowania() != 0) {
        Oblicz_m_SkladowaCalkowania();
    } else {
        set_m_SkladowaCalkowania(0);
    }

    double u_ideal = get_m_SkladowaProporcjonalna() + get_m_SkladowaCalkowania()
                     + get_m_SkladowaRozniczkowania();

    wartosc_pNas = u_ideal;

    double u_sat = u_ideal;

    if (m_czyWindup) {
        if (u_sat > m_UkladMax)
            u_sat = m_UkladMax;
        if (u_sat < m_UkladMin)
            u_sat = m_UkladMin;
    }

    set_m_WartoscSterowania(u_sat);
    set_m_UchybPoprzedni(get_m_Uchyb());

    return u_sat;
}

RegulatorPID::RegulatorPID()
{
    set_m_SkladowaProporcjonalna(0);
    set_m_SkladowaCalkowania(0);
    set_m_SkladowaRozniczkowania(0);
    set_m_WartoscSterowania(0);

    set_m_Wzmocnienie(0);
    set_m_StalaCalkowania(0);
    set_m_StalaRozniczkowania(0);

    set_m_SumaUchybuOdPoczatku(0);
    set_m_UchybPoprzedni(0);
    Zmiana_m_SposobLiczeniaCalki(SposobLiczeniaCalki::Zewnetrzne);
}

RegulatorPID::RegulatorPID(double wzmocnienie, double stalaCalkowania, double stalaRozniczkowania)
{
    set_m_Wzmocnienie(wzmocnienie);
    set_m_StalaCalkowania(stalaCalkowania);
    set_m_StalaRozniczkowania(stalaRozniczkowania);

    set_m_SkladowaProporcjonalna(0);
    set_m_SkladowaCalkowania(0);
    set_m_SkladowaRozniczkowania(0);
    set_m_WartoscSterowania(0);

    set_m_UchybPoprzedni(0);

    set_m_SumaUchybuOdPoczatku(0);
    Zmiana_m_SposobLiczeniaCalki(SposobLiczeniaCalki::Zewnetrzne);
}

RegulatorPID::RegulatorPID(double wzmocnienie, double stalaCalkowania)
{
    set_m_Wzmocnienie(wzmocnienie);
    set_m_StalaCalkowania(stalaCalkowania);

    set_m_SkladowaProporcjonalna(0);
    set_m_SkladowaCalkowania(0);
    set_m_SkladowaRozniczkowania(0);
    set_m_WartoscSterowania(0);

    set_m_UchybPoprzedni(0);
    set_m_StalaRozniczkowania(0);

    set_m_SumaUchybuOdPoczatku(0);
    Zmiana_m_SposobLiczeniaCalki(SposobLiczeniaCalki::Zewnetrzne);
}

RegulatorPID::RegulatorPID(double wzmocnienie)
{
    set_m_SkladowaProporcjonalna(0);
    set_m_SkladowaCalkowania(0);
    set_m_SkladowaRozniczkowania(0);
    set_m_WartoscSterowania(0);
    set_m_UchybPoprzedni(0);
    set_m_Wzmocnienie(wzmocnienie);
    set_m_StalaCalkowania(0);
    set_m_StalaRozniczkowania(0);

    set_m_SumaUchybuOdPoczatku(0);
    Zmiana_m_SposobLiczeniaCalki(SposobLiczeniaCalki::Zewnetrzne);
}

void RegulatorPID::set_m_czyWindup(bool czy)
{
    m_czyWindup = czy;
}
