#include "GeneratorWartosci.h"
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

double GeneratorWartosci::generuj()
{
    // double w;
    // int cykli_na_okres = czasRzeczywisty / (static_cast<double>(czasTaktowania) / 1000.0);
    // double tempSin;

    // w = skladowaStala;
    // if (typSygnalu == TypSygnalu::SINUS) { //sinus
    //     tempSin = krok % cykli_na_okres;
    //     tempSin /= cykli_na_okres;
    //     tempSin *= 2 * M_PI; // zamiana na radiany

    //     w += amplituda * std::sin(tempSin);
    // } else { //kwadrat
    //     if (krok % cykli_na_okres < wypelnienie * cykli_na_okres)
    //         w += amplituda;
    // }
    // krok++;
    // if (krok >= cykli_na_okres)
    //     krok = 0;
    // return w;
    double wartosc = 0.0;
    if(krok % okres == 0) krok = 0;
    switch (typSygnalu) {
    case TypSygnalu::KWADRAT:
        wartosc += ((miejsceWCyklu() < wypelnienie ? 1.0 : 0.0));
        break;
    case TypSygnalu::SINUS:
        wartosc += std::sin(2.0 * M_PI * (miejsceWCyklu()));
        break;
    default:
        break;
    }
    krok++;
    wartosc *= amplituda;
    wartosc += skladowaStala;
    return wartosc;
}

inline double GeneratorWartosci::miejsceWCyklu() noexcept
{
    return (double)krok / okres;
}

QJsonObject GeneratorWartosci::toJSON()
{
    QJsonObject generator;
    throw "not implemented";
    return generator;
}
GeneratorWartosci GeneratorWartosci::fromJSON(QJsonObject json)
{
    throw "not implemented";
}
void GeneratorWartosci::reset()
{
    krok = 0;
}
