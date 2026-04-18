#include "RegulatorOnOff.h"


RegulatorOnOff::RegulatorOnOff(double u_on, double histereza)
{
    if (histereza <= 0)
        this->histereza = 0.0001;
    else
        this->histereza = histereza;

    wartoscSterowania = u_on;
    stan = Stan::Off;
}

double RegulatorOnOff::symuluj(double e)
{
    // Wtedy w��czamy regulator (stan = ON)
    if (stan == Stan::Off && e > histereza) {
        stan = Stan::On;
    }
    //wy��czamy regulator (stan = OFF)
    else if (stan == Stan::On && e < -histereza) {
        stan = Stan::Off;
    }
    // Je�li regulator jest w stanie ON, wyj�cie = uON,
    if (stan == Stan::On) {
        return wartoscSterowania;
    } else {
        return 0.0;
    }
}

double RegulatorOnOff::getWartoscSterowania()
{
    return wartoscSterowania;
}

void RegulatorOnOff::setWartoscSterowania(double wartoscS)
{
    wartoscSterowania = wartoscS;
}

void RegulatorOnOff::reset()
{
    stan = Stan::Off;
}
