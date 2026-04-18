#pragma once
#include "ARXgenerate.h"
#include "RegulatorOnOff.h"
#include "RegulatorPID.h"

class UAR
{
private:
    ARXgenerate &ARX;
    RegulatorPID *PID;
    RegulatorOnOff *OnOff;
    double wW, y1, u; //wW-wartość zadana, y1-y w chwili -1, u-wartość zadana???
    bool choice;      //przełącznik między symulacją OnOff a PID
    bool czyNasycenie = false;
    double nasycenieMax;
    double nasycenieMin;

public:
    enum class RodzajSterowania { OnOff, PID };
    UAR(ARXgenerate &zARX, RegulatorPID &zPID);
    UAR(ARXgenerate &zARX, RegulatorOnOff &zOnOff);

    void setPID(RegulatorPID &zPID);
    void setOnOff(RegulatorOnOff &zOnOff);
    void setCzynasycenie(bool czy);
    void setNasycenieMax(double max);
    void setNasycenieMin(double min);
    bool get_choice();
    double Symuluj(double sygWe);
};
