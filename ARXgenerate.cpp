#include "ARXgenerate.h"

//konstruktory
ARXgenerate::ARXgenerate(vector<double> nA, vector<double> nB, int nk, double nz)
{
    A = nA;
    B = nB;
    k = nk;
    z = nz;
    for (int i = 0; i < k; i++) {
        Uopozniony.push_back(0);
    }
    for (int i = 0; i < B.size(); i++)
        U.push_back(0);
    for (int i = 0; i < A.size(); i++)
        Y.push_back(0);
}
//GETTERY
const vector<double> &ARXgenerate::getA() const
{
    return A;
}

const vector<double> &ARXgenerate::getB() const
{
    return B;
}

deque<double> ARXgenerate::getU() const
{
    return U;
}
double ARXgenerate::getWartoscU()
{
    return U.at(0);
}

deque<double> ARXgenerate::getY() const
{
    return Y;
}
double ARXgenerate::getWartoscY()
{
    return Y.at(0);
}

deque<double> ARXgenerate::getUop() const
{
    return Uopozniony;
}

int ARXgenerate::getK() const
{
    return k;
}

double ARXgenerate::getZ() const
{
    return z;
}

void ARXgenerate::setA(vector<double> a)
{
    Y.clear();
    A = a;
    for (int i = 0; i < A.size(); i++)
        Y.push_back(0);
}

void ARXgenerate::setB(vector<double> b)
{
    U.clear();
    B = b;
    for (int i = 0; i < B.size(); i++)
        U.push_back(0);
}

//settery ograniczenia wartoœci zadanej
void ARXgenerate::setMaxWejscia(double smax)
{
    maxZad = smax;
}
void ARXgenerate::setMinWejscia(double smin)
{
    minZad = smin;
}
//settery ograniczenia wartoœci wyjsciowej
void ARXgenerate::setMaxWyjscia(double smax)
{
    maxReg = smax;
}
void ARXgenerate::setMinWyjscia(double smin)
{
    minReg = smin;
}
void ARXgenerate::setOgraniczenie(bool enabled)
{
    ograniczenia = enabled;
}

//funkcja symuluj¹ca obiekt ARX
double ARXgenerate::symuluj(double u)
{
    int size = B.size() - 1;

    U.pop_front();
    U.push_back(Uopozniony.front());
    if (!ograniczenia) {
        if (u > maxZad)
            Uopozniony.push_back(maxZad);
        else if (u < minZad)
            Uopozniony.push_back(minZad);
        else
            Uopozniony.push_back(u);
    } else {
        Uopozniony.push_back(u);
    }
    Uopozniony.pop_front();

    double b = 0, a = 0, y = 0;

    for (int i = 0; i < B.size(); i++) {
        b += B.at(i) * U.at(size - i); // Mnożenie wektora B przez historię wejść U
    }
    for (int i = 0; i < A.size(); i++) {
        a += A.at(i) * Y.at(size - i); // Mnożenie wektora A przez historię wyjść Y
    }
    y = b - a + z;
    if (!ograniczenia) {
        if (y > maxReg)
            Y.push_back(maxReg);
        else if (y < minReg)
            Y.push_back(minReg);
        else
            Y.push_back(y);
    } else {
        Y.push_back(y);
    }
    Y.pop_front();
    //y=Y.at(Y.front());

    return y;
}

void ARXgenerate::setZaklocenia(double zakl)
{
    z = zakl;
}

void ARXgenerate::setOpoznienie(int op)
{
    k = op;
    aktualizacjaBuforowPoZmianieOpoznienia();
}

void ARXgenerate::aktualizacjaBuforowPoZmianieOpoznienia()
{
    if (Uopozniony.size() == k) {
        return;
    } else if (Uopozniony.size() < k) {
        while (k > Uopozniony.size()) {
            Uopozniony.push_front(Uopozniony.front());
        }
        return;
    } else {
        while (k < Uopozniony.size()) {
            Uopozniony.pop_back();
        }
    }
}
