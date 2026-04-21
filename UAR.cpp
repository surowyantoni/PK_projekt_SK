#include "UAR.h"

UAR::UAR(class ARX *ARX, GeneratorWartosci* gen, RegulatorPID *PID)
    : ARX(ARX)
    , gen(gen)
    , poprzednieWyjscie(0.0)
{
    setPID(PID);
}

UAR::UAR(class ARX *ARX, GeneratorWartosci* gen, RegulatorOnOff *OnOff)
    : ARX(ARX)
    , gen(gen)
    , poprzednieWyjscie(0.0)
{
    setOnOff(OnOff);
}

void UAR::setPID(RegulatorPID* PID)
{
    this->PID = PID;
    OnOff = nullptr;
}

void UAR::setOnOff(RegulatorOnOff* OnOff)
{
    this->OnOff = OnOff;
    PID = nullptr;
}

double UAR::symuluj(double wartoscZadana)
{
    switch (getWybranyRegulator())
    {
    case RodzajSterowania::PID:
        wartoscZadana = PID->symuluj(wartoscZadana - poprzednieWyjscie);
        break;
    case RodzajSterowania::OnOff:
        wartoscZadana = OnOff->symuluj(wartoscZadana - poprzednieWyjscie);
        break;
    }
    poprzednieWyjscie = ARX->symuluj(wartoscZadana);

    return poprzednieWyjscie;
}

UAR::Tick UAR::symuluj()
{
    Tick tick;
    double wartoscZadana = gen->generuj();
    tick.wartoscZadana = wartoscZadana;

    tick.uchyb = wartoscZadana - poprzednieWyjscie;

    switch (getWybranyRegulator())
    {
    case RodzajSterowania::PID:
        tick.pid = PID->symuluj(tick.uchyb);
        tick.sterowanie = PID->limityWyjscia.clamp(tick.pid.value());
        break;
    case RodzajSterowania::OnOff:
        tick.pid = std::nullopt;
        tick.sterowanie = OnOff->symuluj(tick.uchyb);
        break;
    }

    tick.wartoscRegulowana = ARX->symuluj(tick.sterowanie);
    poprzednieWyjscie = tick.wartoscRegulowana;
    return tick;
}

UAR::RodzajSterowania UAR::getWybranyRegulator()
{
    if(PID != nullptr)
        return UAR::RodzajSterowania::PID;
    else if(OnOff != nullptr)
        return UAR::RodzajSterowania::OnOff;
    throw "Mamy problemik";
}
