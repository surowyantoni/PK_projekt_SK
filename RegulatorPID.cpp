#include "RegulatorPID.h"
#include "qdebug.h"
#include "qglobal.h"
#include <format>


PIDTick::operator double() const noexcept
{
    return P + I + D;
}

PIDTick RegulatorPID::symuluj(double uchyb)
{
    PIDTick tick;

    tick.P = k * uchyb;

    if(Ti == 0.0)
    {
        tick.I = 0.0;
    }
    else
    {
        sumaUchybowCalkowanieZewnetrzne += uchyb;
        sumaUchybowCalkowanieWewnetrzne += (double)uchyb / Ti;
        switch (sposobLiczeniaCalki)
        {
        case Zewnetrzne:
            tick.I = (double)sumaUchybowCalkowanieZewnetrzne / Ti;
            break;
        case Wewnetrzne:
            tick.I = sumaUchybowCalkowanieWewnetrzne;
            break;
        }
    }



    tick.D = Td * (uchyb - poprzedniUchyb);
    // qDebug() << std::format("P= {} I = {} D= {} ", tick.P, tick.I, tick.D).c_str();
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
        sumaUchybowCalkowanieWewnetrzne = sumaUchybowCalkowanieZewnetrzne / Ti;
        break;
    case SposobLiczeniaCalki::Zewnetrzne:
        sumaUchybowCalkowanieZewnetrzne = sumaUchybowCalkowanieWewnetrzne * Ti;
        break;
    default:
        assert(1 == 0); // wyrzuć błąd
        break;
    }
}
RegulatorPID::RegulatorPID(double k, double Ti, double Td,
             MinMaxClamp ograniczenia, bool antiWindupActive)
    : limityWyjscia(ograniczenia)
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
