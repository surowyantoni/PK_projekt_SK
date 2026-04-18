#include "ARXgenerate.h"
#include "qjsonarray.h"

//konstruktory
ARXgenerate::ARXgenerate(vector<double> nA, vector<double> nB, int nk, double nz)
    :
    limityZadana(MinMaxClamp(-10.0, 10.0)),
    limityRegulowana(MinMaxClamp(-10.0, 10.0))
{
    A = nA;
    B = nB;
    k = nk;
    z = nz;
    Uopozniony.resize(k);
    U.resize(B.size());
    Y.resize(A.size());


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
    limityZadana.setMax(smax);
}
void ARXgenerate::setMinWejscia(double smin)
{
    limityZadana.setMin(smin);
}
//settery ograniczenia wartoœci wyjsciowej
void ARXgenerate::setMaxWyjscia(double smax)
{
    limityRegulowana.setMax(smax);
}
void ARXgenerate::setMinWyjscia(double smin)
{
    limityRegulowana.setMin(smin);
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
    if (ograniczenia)
        u = limityZadana.clamp(u);
    Uopozniony.push_back(u);
    Uopozniony.pop_front();

    double b = 0, a = 0, y = 0;

    for (int i = 0; i < B.size(); i++) {
        b += B.at(i) * U.at(size - i); // Mnożenie wektora B przez historię wejść U
    }
    for (int i = 0; i < A.size(); i++) {
        a += A.at(i) * Y.at(size - i); // Mnożenie wektora A przez historię wyjść Y
    }
    y = b - a + z;
    if (ograniczenia)
        y = limityRegulowana.clamp(y);
    Y.push_back(y);
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

QJsonObject ARXgenerate::toJSON()
{
    QJsonObject arx;
    arx["k"] = k;
    arx["z"] = z;
    arx["A"] = QJsonArray();
    for (auto &el : A)
        arx["A"].toArray().append(el);
    arx["B"] = QJsonArray();
    for (auto &el : A)
        arx["B"].toArray().append(el);
    arx["wej"] = QJsonObject();
    arx["wej"].toObject().insert("min", limityZadana.getMin());
    arx["wej"].toObject().insert("max", limityZadana.getMax());
    arx["wyj"] = QJsonObject();
    arx["wyj"].toObject().insert("min", limityRegulowana.getMin());
    arx["wyj"].toObject().insert("max", limityRegulowana.getMax());
    arx["ogranicznia"] = ograniczenia;
    return arx;
}
