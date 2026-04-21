#include "RegulatorPID.h"


PIDTick::operator double() const noexcept
{
    return P + I + D;
}

PIDTick RegulatorPID::symuluj(double uchyb)
{
    PIDTick tick;
    tick.P = k * uchyb;

    sumaUchybowCalkowanieWewnetrzne += (double)uchyb / Ti;
    sumaUchybowCalkowanieZewnetrzne += uchyb;

    switch (sposobLiczeniaCalki)
    {
    case Zewnetrzne:
        tick.I = (double)sumaUchybowCalkowanieZewnetrzne / Ti;
        break;
    case Wewnetrzne:
        tick.I = sumaUchybowCalkowanieWewnetrzne;
        break;
    }

    tick.D = Td * (uchyb - poprzedniUchyb);
    poprzedniUchyb = uchyb;

    return tick;
}

void RegulatorPID::zmienSposobLiczeniaCalki(SposobLiczeniaCalki value)
{
    // nic do roboty
    if(value == this->sposobLiczeniaCalki)
        return;

    switch (value)
    {
    case SposobLiczeniaCalki::Wewnetrzne:
        sumaUchybowCalkowanieZewnetrzne = (double)sumaUchybowCalkowanieWewnetrzne / Ti;
        break;
    case SposobLiczeniaCalki::Zewnetrzne:
        sumaUchybowCalkowanieWewnetrzne = sumaUchybowCalkowanieZewnetrzne * Ti;
        break;
    default:
        assert(1 == 0); // wyrzuć błąd
        break;
    }
}
RegulatorPID::RegulatorPID(double k, double Ti, double Td,
             double antiWindup, double antiWindupMin, double antiWindupMax,
             bool antiWindupActive)
    : antiWindupMax(this, antiWindupMax)
    , antiWindupMin(this, antiWindupMin)
    , antiWindupWspolczynnik(antiWindup)
    , antiWindupActive(antiWindupActive)
    , k(k)
    , Ti(Ti)
    , Td(Td)
    , sposobLiczeniaCalki(this, SposobLiczeniaCalki::Wewnetrzne)
    , poprzedniUchyb(0.0)
    , sumaUchybowCalkowanieZewnetrzne(0.0)
    , sumaUchybowCalkowanieWewnetrzne(0.0)
{

}
void RegulatorPID::reset()
{
    poprzedniUchyb = 0.0;
    resetCzesciCalkujacej();
}
void RegulatorPID::resetCzesciCalkujacej()
{
    sumaUchybowCalkowanieWewnetrzne = 0.0;
    sumaUchybowCalkowanieZewnetrzne = 0.0;
}
