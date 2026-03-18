#include "RegulatorOnOff.h"

// Konstruktor klasy
RegulatorOnOff::RegulatorOnOff()
{
    h = 0.001;
    uON = 1;
    stan = false;
}
RegulatorOnOff::RegulatorOnOff(double u_on, double histereza)
{
    if (histereza <= 0)
        h = 0.0001;
    else
        h = histereza;

    uON = u_on;
    stan = false;
}

double RegulatorOnOff::symuluj(double e)
{
    // Wtedy w��czamy regulator (stan = ON)
    if (stan == false && e > h) {
        stan = true;
    }
    //wy��czamy regulator (stan = OFF)
    else if (stan && e < -h) {
        stan = false;
    }
    // Je�li regulator jest w stanie ON, wyj�cie = uON,
    if (stan) {
        set_wartoscSterowania(uON);
        return uON;
    } else {
        set_wartoscSterowania(0.0);
        return 0.0;
    }
}

double RegulatorOnOff::get_wartoscSterowania()
{
    return wartoscSterowania;
}

void RegulatorOnOff::set_wartoscSterowania(double wartoscS)
{
    wartoscSterowania = wartoscS;
}

void RegulatorOnOff::Reset()
{
    stan = false;
}
