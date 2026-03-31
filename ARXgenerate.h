#pragma once

#include "qjsonobject.h"
#include <deque>
#include <vector>

using namespace std;

class ARXgenerate
{
private:
    vector<double> A, B; //wspolczynniki wektorow A i B
    int k = 1; // Opoznienie transportowe
    double z = 0; // Wartosc srodkowa zaklocen
    deque<double> U; // Sygnal wejsciowy
    deque<double> Y; // Sygnal wyjsciowy
    deque<double> Uopozniony; // Opozniony sygnal sejsciowy
    bool ograniczenia = true; //czy włączone są ograniczenia wartości
    double maxZad = 10, minZad = -10;
    double maxReg = 10, minReg = -10;
    void aktualizacjaBuforowPoZmianieOpoznienia();

public:

    //konstruktory
    ARXgenerate(vector<double> nA = {0}, vector<double> nB = {0}, int nk = 1, double nz = 0); //nz=warto�� zak��cenia

    //gettery
    const vector<double> &getA() const;
    const vector<double> &getB() const;
    deque<double> getU() const;
    deque<double> getY() const;
    deque<double> getUop() const;
    int getK() const;
    double getZ() const;
    double getWartoscU();
    double getWartoscY();

    //settery
    void setA(vector<double> a);
    void setB(vector<double> b);
    void setMaxWejscia(double smax);
    void setMinWejscia(double smin);
    void setMaxWyjscia(double smax);
    void setMinWyjscia(double smin);
    void setOgraniczenie(bool enabled);
    void setZaklocenia(double zakl);
    void setOpoznienie(int op);

    double symuluj(double u);

    QJsonObject toJSON();
};
