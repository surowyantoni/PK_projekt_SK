#include "UAR.h"

UAR::UAR(ARXgenerate &zARX, RegulatorPID &zPID)
    : ARX(zARX)
    , PID(&zPID)
    , OnOff(nullptr)
    , wW(0)
    , u(0)
    , y1(0)
    , choice(0)
{}

UAR::UAR(ARXgenerate &zARX, RegulatorOnOff &zOnOff)
    : ARX(zARX)
    , PID(nullptr)
    , OnOff(&zOnOff)
    , wW(0)
    , u(0)
    , y1(0)
    , choice(1)
{}

void UAR::setPID(RegulatorPID &zPID)
{
    PID = &zPID;
    OnOff = nullptr;
    choice = 0;
}

void UAR::setOnOff(RegulatorOnOff &zOnOff)
{
    PID = nullptr;
    OnOff = &zOnOff;
    choice = 1;
}

double UAR::Symuluj(double sygWe)
{
    if (choice == 0) {
        wW = PID->Symuluj(sygWe - y1);
        y1 = ARX.rozpocznij(wW);
        //cout << y1<<" ";
        if (czyNasycenie) {
            if (y1 > nasycenieMax)
                y1 = nasycenieMax;
            else if (y1 < nasycenieMin)
                y1 = nasycenieMin;
        }
    } else if (choice == 1) {
        wW = OnOff->symuluj(sygWe - y1);
        y1 = ARX.rozpocznij(wW);
        //cout << y1<<" ";
        if (czyNasycenie) {
            if (czyNasycenie) {
                if (y1 > nasycenieMax)
                    y1 = nasycenieMax;
                else if (y1 < nasycenieMin)
                    y1 = nasycenieMin;
            }
        }
    }
    return y1;
}

bool UAR::get_choice()
{
    return choice;
}

void UAR::setCzynasycenie(bool czy)
{
    czyNasycenie = czy;
}
void UAR::setNasycenieMax(double max)
{
    nasycenieMax = max;
}
void UAR::setNasycenieMin(double min)
{
    nasycenieMin = min;
}
