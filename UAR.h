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
    UAR(ARX* ARX, GeneratorWartosci* gen, RegulatorPID* PID);
    UAR(ARX* ARX, GeneratorWartosci* gen, RegulatorOnOff* OnOff);

    void setPID(RegulatorPID* PID);
    void setOnOff(RegulatorOnOff* OnOff);
    RodzajSterowania getWybranyRegulator();
    Tick symuluj();
    double symuluj(double wartZadana); // TYLKO do pisania testów
private:
    ARX* ARX;
    RegulatorPID* PID;
    RegulatorOnOff* OnOff;
    GeneratorWartosci* gen;
    double poprzednieWyjscie;
};
