#include "GenW.h"
#define _USE_MATH_DEFINES
#include <cmath>

GenW::GenW()
{
    czasRzeczywisty = 10;
    czasTaktowania = 200;
    amplituda = 1;
    skladowaStala = 1;
    wypelnienie = 1;
    typSygnalu = Typ::SINUS;
    krok = 0;
}

void GenW::setCzasTakt(int Tt)
{
    czasTaktowania = Tt;
}

void GenW::setCzasRzecz(int Tr)
{
    czasRzeczywisty = Tr;
}

void GenW::setAmplituda(double a)
{
    amplituda = a;
    //aezekmi
}

void GenW::setSkladoaStala(double s)
{
    skladowaStala = s;
}

void GenW::setWypelnienie(double p)
{
    wypelnienie = p;
}

void GenW::setTyp(Typ c)
{
    typSygnalu = c;
}

double GenW::generuj()
{
    double w;
    int cykli_na_okres = czasRzeczywisty / (static_cast<double>(czasTaktowania) / 1000.0);
    double tempSin;

    w = skladowaStala;
    if (typSygnalu == Typ::SINUS)
    { //sinus
        tempSin = krok % cykli_na_okres;
        tempSin /= cykli_na_okres;
        tempSin *= 2 * M_PI; // zamiana na radiany

        w += amplituda * std::sin(tempSin);
    } else
    { //kwadrat
        if (krok % cykli_na_okres < wypelnienie * cykli_na_okres)
            w += amplituda;
    }
    krok++;
    if (krok >= cykli_na_okres)
        krok = 0;
    return w;
}

QJsonObject GenW::toJSON()
{
    QJsonObject generator;
    generator["typ"] = (int)this->typSygnalu;
    generator["wypelnienie"] = this->wypelnienie;
    generator["skladowaStala"] = this->skladowaStala;
    generator["amplituda"] = this->amplituda;
    generator["czasRzeczywisty"] = this->czasRzeczywisty;
    generator["czasTaktowania"] = this->czasTaktowania;

    return generator;
}
