#pragma once

class RegulatorOnOff
{
private:
    double h;
    double uON; //
    bool stan;
    double wartoscSterowania;

public:
    RegulatorOnOff();
    RegulatorOnOff(double u_on, double histereza);
    double symuluj(double e);
    double get_wartoscSterowania();
    void set_wartoscSterowania(double wartoscS);
    void Reset();
};
