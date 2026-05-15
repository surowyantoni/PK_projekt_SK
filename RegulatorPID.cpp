#include "RegulatorPID.h"
#include "qdebug.h"
#include "qglobal.h"
#include <qiodevice.h>


PIDTick::operator double() const noexcept
{
    return P + I + D;
}

PIDTick RegulatorPID::symuluj(double uchyb)
{
    PIDTick tick;

    tick.P = k.get() * uchyb;
    tick.D = Td.get() * (uchyb - poprzedniUchyb);
    poprzedniUchyb = uchyb;

    if(Ti.get() == 0.0)
    {
        tick.I = 0.0;
        return tick;
    }


    sumaUchybowCalkowanieZewnetrzne += uchyb;
    sumaUchybowCalkowanieWewnetrzne += uchyb / Ti.get();
    switch (sposobLiczeniaCalki.get())
    {
    case Zewnetrzne:
        tick.I = (double)sumaUchybowCalkowanieZewnetrzne / Ti.get();
        break;
    case Wewnetrzne:
        tick.I = sumaUchybowCalkowanieWewnetrzne;
        break;
    }



    double sterowanie_przed_filtrami = (double)tick;
    if (antiWindupActive.get()) {
        if (sterowanie_przed_filtrami > limityWyjscia.getMax())
        {
            sumaUchybowCalkowanieZewnetrzne -= uchyb;
            sumaUchybowCalkowanieWewnetrzne -= uchyb / Ti.get();
        }
        else if (sterowanie_przed_filtrami < limityWyjscia.getMin())
        {
            sumaUchybowCalkowanieZewnetrzne -= uchyb;
            sumaUchybowCalkowanieWewnetrzne -= uchyb / Ti.get();
        }
        switch (sposobLiczeniaCalki.get())
        {
        case Zewnetrzne:
            tick.I = (double)sumaUchybowCalkowanieZewnetrzne / Ti.get();
            break;
        case Wewnetrzne:
            tick.I = sumaUchybowCalkowanieWewnetrzne;
            break;
        }
    }

    return tick;
}

void RegulatorPID::zmienSposobLiczeniaCalki(SposobLiczeniaCalki value)
{
    // nic do roboty
    if(value == sposobLiczeniaCalki.get())
        return;
    if (Ti.get() != 0.0)
        return;


    switch (value)
    {
    case SposobLiczeniaCalki::Wewnetrzne:
            sumaUchybowCalkowanieWewnetrzne = sumaUchybowCalkowanieZewnetrzne / Ti.get();
        break;
    case SposobLiczeniaCalki::Zewnetrzne:
        sumaUchybowCalkowanieZewnetrzne = sumaUchybowCalkowanieWewnetrzne * Ti.get();
        break;
    }
}
RegulatorPID::RegulatorPID(double k, double Ti, double Td,
             MinMaxClamp ograniczenia, bool antiWindupActive)
    : limityWyjscia(ograniczenia)
    , antiWindupActive(antiWindupActive)
    , k{k, this}
    , Ti{Ti, this}
    , Td{Td, this}
    , sposobLiczeniaCalki{(SposobLiczeniaCalki)CONSTS::PID::rodzaj_calkowania, this}
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

QJsonObject RegulatorPID::toJSON() const
{
    return QJsonObject();
}
void RegulatorPID::fromJSON(QJsonObject& json)
{

}
QByteArray RegulatorPID::toByteArray() const
{
    QByteArray data;
    QDataStream s(&data, QIODevice::WriteOnly);
    s << k.get() << Ti.get() << Td.get() << sposobLiczeniaCalki.get()
      << limityWyjscia.getMin() << limityWyjscia.getMax()
      << limityWyjscia.getActive() << antiWindupActive.get();
    return data;
}
void RegulatorPID::fromByteArray(QByteArray& data)
{
    QDataStream s(&data, QIODevice::ReadOnly);
    double limMax, limMin;
    bool lim;
    s >> k.value >> Ti.value >> Td.value >> sposobLiczeniaCalki.value
        >> limMin >> limMax >> lim >> antiWindupActive.value;
    limityWyjscia = MinMaxClamp(limMin, limMax, lim);
}
