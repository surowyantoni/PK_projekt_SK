#include "RegulatorOnOff.h"


RegulatorOnOff::RegulatorOnOff(double wartoscSterowania, double histereza)
    : stan(Stan::Off)
    , histereza(histereza)
    , wartoscSterowania(wartoscSterowania)
{
}

double RegulatorOnOff::symuluj(double uchyb)
{
    if (stan == Stan::Off && uchyb > histereza)
        stan = Stan::On;
    else if (stan == Stan::On && uchyb < -histereza)
        stan = Stan::Off;

    switch (stan)
    {
    case Stan::Off:
        return 0.0;
        break;
    case Stan::On:
        return wartoscSterowania;
        break;
    }
}

void RegulatorOnOff::reset()
{
    stan = Stan::Off;
}
