#include "UAR.h"


UAR::UAR(class ARX *ARX, GeneratorWartosci* gen, RegulatorPID* pid, RegulatorOnOff *OnOff, RodzajSterowania rodzaj)
    : regulator(rodzaj)
    , ARX(ARX)
    , PID(pid)
    , OnOff(OnOff)
    , gen(gen)
    , poprzednieWyjscie(0.0)
{}


double UAR::symuluj(double wartoscZadana)
{
    switch (regulator)
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

UAR::Tick UAR::symuluj(uint32_t interwal)
{
    Tick tick;
    double wartoscZadana = gen->generuj(interwal);
    tick.wartoscZadana = wartoscZadana;

    tick.uchyb = wartoscZadana - poprzednieWyjscie;

    switch (regulator)
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
double UAR::symulujObiekt(double sterowanie)
{
    return ARX->symuluj(sterowanie);
}

void UAR::zaktualizujPoprzendieWyjscie(double wyjscie)
{
    poprzednieWyjscie = wyjscie;
}
UAR::Tick UAR::symulujBezObiektu(uint32_t interwal)
{
    Tick tick;
    tick.wartoscZadana = gen->generuj(interwal);

    tick.uchyb = tick.wartoscZadana - poprzednieWyjscie;

    switch (regulator)
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

    // Nie ustawione, bo dostenimy wartość dopiero gdy obiekt odpowie
    tick.wartoscRegulowana = 0.0;
    return tick;
}

