#include "GeneratorWartosci.h"
#include "qabstractsocket.h"
#define _USE_MATH_DEFINES
#include <cmath>

GeneratorWartosci::GeneratorWartosci(double amplituda, double skladowaStala, uint32_t okres, TypSygnalu typSygnalu, double wypelnienie)
:
    okres(okres),
    amplituda(amplituda),
    skladowaStala (skladowaStala),
    wypelnienie (wypelnienie),
    typSygnalu (typSygnalu),
    krok (0)
{}

double GeneratorWartosci::generuj(uint32_t przeskok)
{
    double wartosc = 0.0;
    if(krok > okres.get()) krok = okres.get() % krok;
    switch (typSygnalu.get()) {
    case TypSygnalu::KWADRAT:
        wartosc += ((miejsceWCyklu() < wypelnienie.get() ? 1.0 : 0.0));
        break;
    case TypSygnalu::SINUS:
        wartosc += std::sin(2.0 * M_PI * (miejsceWCyklu()));
        break;
    default:
        break;
    }
    krok+= przeskok;
    wartosc *= amplituda.get();
    wartosc += skladowaStala.get();
    return wartosc;
}

inline double GeneratorWartosci::miejsceWCyklu() noexcept
{
    return (double)krok / okres.get();
}


void GeneratorWartosci::reset()
{
    krok = 0;
}


QJsonObject GeneratorWartosci::toJSON() const
{
    return QJsonObject();
}
void GeneratorWartosci::fromJSON(QJsonObject& json)
{

}
QByteArray GeneratorWartosci::toByteArray() const
{
    QByteArray data;
    QDataStream s(&data, QIODevice::WriteOnly);
    s << okres.get() << amplituda.get() << skladowaStala.get() << wypelnienie.get()
      << typSygnalu.get();
    return data;
}
void GeneratorWartosci::fromByteArray(QByteArray& data)
{
    QDataStream s(&data, QIODevice::ReadOnly);

    s >> okres.value >> amplituda.value >> skladowaStala.value >> wypelnienie.value
        >> typSygnalu.value;
}
