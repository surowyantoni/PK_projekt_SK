#pragma once
#include "ARX.hpp"
#include "GeneratorWartosci.h"
#include "RegulatorOnOff.h"
#include "RegulatorPID.h"

class UAR
{
public:
    struct Tick
    {
        std::optional<PIDTick> pid;
        double sterowanie;
        double uchyb;
        double wartoscRegulowana;
        double wartoscZadana;
    };
    enum class RodzajSterowania { OnOff, PID };
    UAR(ARX* ARX, GeneratorWartosci* gen, RegulatorPID* PID, RegulatorOnOff* OnOff, RodzajSterowania rodzaj);

    Tick symuluj(uint32_t interwal);
    double symuluj(double wartZadana); // TYLKO do pisania testów
    double symulujObiekt(double wejscie);
    Tick symulujBezObiektu(uint32_t interwal);
    void zaktualizujPoprzendieWyjscie(double wyjscie);
    RodzajSterowania regulator;
private:
    ARX* ARX;
    RegulatorPID* PID;
    RegulatorOnOff* OnOff;
    GeneratorWartosci* gen;
    double poprzednieWyjscie;
};
