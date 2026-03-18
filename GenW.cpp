#include "GenW.h"

GenW::GenW()
{
    czasRzeczywisty = 10;
    czasTaktowania = 200;
    A = 1;
    S = 1;
    P = 1;
    choice = 0;
    i = 0;
}

void GenW::setCzasTakt(int Tt)
{
    czasTaktowania = Tt;
}

void GenW::setCzasRzecz(int Tr)
{
    czasRzeczywisty = Tr;
}

void GenW::setA(int a)
{
    A = a;
    //aezekmi
}

void GenW::setS(int s)
{
    S = s;
}

void GenW::setP(double p)
{
    P = p;
}

void GenW::setChoice(int c)
{
    choice = c;
}

double GenW::obliczWartosc()
{
    double w;
    int T = floor(czasRzeczywisty / (static_cast<double>(czasTaktowania) / 1000.0));
    //cout << T << endl;
    double tempSin;
    double tempW;
    if (choice) { //sinus
        tempSin = i % T;
        //cout << tempSin << "\t";
        tempSin /= T;
        //cout << tempSin << "\t";
        tempSin *= 2 * 3.1415;
        //cout << tempSin << "\t";
        tempW = A * sin(tempSin) + S;
        //cout << tempW << "\n";
        w = tempW;
    } else { //kwadrat
        if (i % T < P * T)
            w = (A + S);
        else
            w = (S);
    }
    i++;
    if (i >= T)
        i = 0;
    return w;
}
