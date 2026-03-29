#pragma once

#include <deque>
#include <iostream>
#include <vector>

using namespace std;

class ARXgenerate
{
private:
    vector<double> A = {0}, B = {0}; //wsp��czynniki wektor�w a i b
    double k = 1, z = 0;             //k=op��nienie transportowe; z=warto�� zak��ce�
    deque<double> U, Y,        Uop; //U=sygna� wej�ciowy; Y=sygna� wyj�ciowy; Uop=op��niony sygna� wej�ciowy o 'k'
    bool check = true; //czy limity sprawdzaj�ca warunek
    double maxZad = 10, minZad = -10;
    double maxReg = 10, minReg = -10;

public:
    //zmienne b�edu

    //konstruktory
    ARXgenerate();
    ARXgenerate(vector<double> nA, vector<double> nB);         //nA=wielomian A, nB=wielomian B
    ARXgenerate(vector<double> nA, vector<double> nB, int nk); //nk=op��nienie transportowe
    ARXgenerate(vector<double> nA, vector<double> nB, int nk, double nz); //nz=warto�� zak��cenia

    //gettery
    const vector<double> &getA() const;
    const vector<double> &getB() const;
    deque<double> getU() const;
    deque<double> getY() const;
    deque<double> getUop() const;
    double getK() const;
    double getZ() const;
    double getWartoscU();
    double getWartoscY();

    //settery
    void setA(vector<double> a);
    void setB(vector<double> b);
    void setMaxZad(double smax);
    void setMinZad(double smin);
    void setMaxReg(double smax);
    void setMinReg(double smin);
    void setOgraniczenie(bool choice);
    void setZaklocenia(double zakl);
    void setOpoznienie(double op);

    double rozpocznij(double u);
    void zmienOpoznienie();
};
