#include "RegulatorOnOff.h"
#include <QIODevice>


RegulatorOnOff::RegulatorOnOff(double wartoscSterowania, double histereza)
    : stan(Stan::Off)
    , histereza(histereza, this)
    , wartoscSterowania(wartoscSterowania, this)
{
}

double RegulatorOnOff::symuluj(double uchyb)
{
    if (stan == Stan::Off && uchyb > histereza.get())
        stan = Stan::On;
    else if (stan == Stan::On && uchyb < -histereza.get())
        stan = Stan::Off;

    switch (stan)
    {
    case Stan::Off:
        return 0.0;
        break;
    case Stan::On:
        return wartoscSterowania.get();
        break;
    }
}

void RegulatorOnOff::reset()
{
    stan = Stan::Off;
}

QJsonObject RegulatorOnOff::toJSON() const
{
    return QJsonObject();
}
void RegulatorOnOff::fromJSON(QJsonObject& json)
{

}
QByteArray RegulatorOnOff::toByteArray() const
{
    QByteArray data;
    QDataStream s(&data, QIODevice::WriteOnly);
    s << wartoscSterowania.get() << histereza.get();
    return data;
}
void RegulatorOnOff::fromByteArray(QByteArray& data)
{
    QDataStream s(&data, QIODevice::ReadOnly);
    s >> wartoscSterowania.value >> histereza.value;
}
