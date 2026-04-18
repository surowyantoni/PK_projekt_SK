#pragma once

class RegulatorOnOff
{
    enum class Stan {Off, On};


    double histereza;
    Stan stan;
    double wartoscSterowania;

public:
    RegulatorOnOff(double wartoscSterowania = 1.0, double histereza = 0.001);
    double symuluj(double e);
    double getWartoscSterowania();
    void setWartoscSterowania(double wartoscSterowania);
    void reset();
};
