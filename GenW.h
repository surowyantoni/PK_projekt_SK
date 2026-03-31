#pragma once

#include "qjsonobject.h"
class GenW
{
public:
    enum Typ {
        KWADRAT = 0,
        SINUS = 1,
    };
    GenW();
    void setCzasTakt(int Tt);
    void setCzasRzecz(int Tr);
    void setAmplituda(double a);
    void setSkladoaStala(double s);
    void setWypelnienie(double p);
    void setTyp(Typ c);
    double generuj();
    QJsonObject toJSON();

private:
    int czasTaktowania, czasRzeczywisty;
    double amplituda, skladowaStala;
    Typ typSygnalu;     //0-kwadrat; 1-sinus
    double wypelnienie; //wype³nienie
    int krok;           //numer kroku
};
