#pragma once

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

class GenW
{
private:
    int czasTaktowania, czasRzeczywisty, A, S; //A-amplituda, S-sta³a czasowa
    bool choice;                               //0-kwadrat; 1-sinus
    double P;                                  //wype³nienie
    int i;                                     //numer kroku
public:
    GenW();
    void setCzasTakt(int Tt);
    void setCzasRzecz(int Tr);
    void setA(int a);
    void setS(int s);
    void setP(double p);
    void setChoice(int c); //0-kwadrat; 1-sinus
    double obliczWartosc();
};
