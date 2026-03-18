#include "ARXgenerate.h"

//konstruktory
ARXgenerate::ARXgenerate()
{
    k = 1;
    z = 0;
    Uop.push_back(0);
    U.push_back(0);
    Y.push_back(0);
}
ARXgenerate::ARXgenerate(vector<double> nA, vector<double> nB)
{
    A = nA;
    B = nB;
    k = 1;
    z = 0;
    for (int i = 0; i < k; i++) {
        Uop.push_back(0);
    }
    for (int i = 0; i < B.size(); i++)
        U.push_back(0);
    for (int i = 0; i < A.size(); i++)
        Y.push_back(0);
}
ARXgenerate::ARXgenerate(vector<double> nA, vector<double> nB, int nk)
{
    A = nA;
    B = nB;
    k = nk;
    z = 0;
    for (int i = 0; i < k; i++) {
        Uop.push_back(0);
    }
    for (int i = 0; i < B.size(); i++)
        U.push_back(0);
    for (int i = 0; i < A.size(); i++)
        Y.push_back(0);
}
ARXgenerate::ARXgenerate(vector<double> nA, vector<double> nB, int nk, double nz)
{
    A = nA;
    B = nB;
    k = nk;
    z = nz;
    for (int i = 0; i < k; i++) {
        Uop.push_back(0);
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
    return Uop;
}

double ARXgenerate::getK() const
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
void ARXgenerate::setMaxZad(double smax)
{
    maxZad = smax;
}
void ARXgenerate::setMinZad(double smin)
{
    minZad = smin;
}
//settery ograniczenia wartoœci wyjsciowej
void ARXgenerate::setMaxReg(double smax)
{
    maxReg = smax;
}
void ARXgenerate::setMinReg(double smin)
{
    minReg = smin;
}
void ARXgenerate::setOgraniczenie(bool choice)
{
    check = choice;
}

//funkcja symuluj¹ca obiekt ARX
double ARXgenerate::rozpocznij(double u)
{
    //if (!check()) {
    //	cerr << "Bledne wartosci a i b";
    //	return -100;
    //}
    int size = B.size() - 1;

    U.pop_front();
    U.push_back(Uop.front());
    if (!check) {
        if (u > maxZad)
            Uop.push_back(maxZad);
        else if (u < minZad)
            Uop.push_back(minZad);
        else
            Uop.push_back(u);
    } else {
        Uop.push_back(u);
    }
    Uop.pop_front();

    double b = 0, a = 0, y = 0;

    for (int i = 0; i < B.size(); i++) {
        b += B.at(i) * U.at(size - i); // Mnożenie wektora B przez historię wejść U
    }
    for (int i = 0; i < A.size(); i++) {
        a += A.at(i) * Y.at(size - i); // Mnożenie wektora A przez historię wyjść Y
    }
    y = b - a + z;
    if (!check) {
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

void ARXgenerate::setOpoznienie(double op)
{
    k = op;
    zmienOpoznienie();
}

void ARXgenerate::zmienOpoznienie()
{
    if (Uop.size() == k) {
        return;
    } else if (Uop.size() < k) {
        while (k > Uop.size()) {
            Uop.push_front(Uop.front());
        }
        return;
    } else {
        while (k < Uop.size()) {
            Uop.pop_back();
        }
    }
}
