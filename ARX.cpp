#include "ARX.hpp"
#include <random>

//konstruktory
ARX::ARX(vector<double> nA, vector<double> nB, int nk, double nz)
    : limityZadana(MinMaxClamp(-10.0, 10.0))
    , limityRegulowana(MinMaxClamp(-10.0, 10.0))
    , k(this, nk)
    , z(nz)
    , U(std::deque<double>())
    , Uopozniony(std::deque<double>())
    , Y(std::deque<double>())
    , A(this, nA)
    , B(this, nB)
{
    Uopozniony.get().resize(k);
    U.get().resize(B.get().size());
    Y.get().resize(A.get().size());


    for (size_t i = 0; i < k; i++) {
        Uopozniony.get().push_back(0.0);
    }
    for (size_t i = 0; i < B.get().size(); i++)
        U.get().push_back(0.0);
    for (size_t i = 0; i < A.get().size(); i++)
        Y.get().push_back(0.0);
}

ARX ARX::fromJSON(QJsonObject& json)
{
    return ARX();
}
double ARX::generujZaklocenie()
{
    static std::random_device rd;
    static std::mt19937 generator(rd());

    if(z == 0)
        return 0.0;

    std::normal_distribution<double> distrib(0.0, z);
    return distrib(generator);
}

void ARX::reset()
{
    U.get().clear();
    Y.get().clear();
    Uopozniony.get().clear();

    Uopozniony.get().resize(k);
    U.get().resize(B.get().size());
    Y.get().resize(A.get().size());


    for (size_t i = 0; i < k; i++) {
        Uopozniony.get().push_back(0.0);
    }
    for (size_t i = 0; i < B.get().size(); i++)
        U.get().push_back(0.0);
    for (size_t i = 0; i < A.get().size(); i++)
        Y.get().push_back(0.0);
}
double ARX::symuluj(double u)
{
    int size = B.get().size() - 1;

    U.get().pop_front();
    U.get().push_back(Uopozniony.get().front());
    u = limityZadana.clamp(u);

    Uopozniony.get().push_back(u);
    Uopozniony.get().pop_front();

    double b = 0.0, a = 0.0, y = 0.0;

    for (int i = 0; i < B.get().size(); i++)
    {
        b += B.get()[i] * U.get()[size - i]; // Mnożenie wektora B przez historię wejść U
        a += A.get()[i] * Y.get()[size - i]; // Mnożenie wektora A przez historię wyjść Y
    }
    y = b - a + generujZaklocenie();

    y = limityRegulowana.clamp(y);

    Y.get().push_back(y);
    Y.get().pop_front();
    return y;
}


void ARX::aktualizacjaBuforowPoZmianieOpoznienia()
{
    if (Uopozniony.get().size() == k) {
        return;
    } else if (Uopozniony.get().size() < k) {
        while (k > Uopozniony.get().size()) {
            Uopozniony.get().push_front(Uopozniony.get().front());
        }
        return;
    } else {
        while (k < Uopozniony.get().size()) {
            Uopozniony.get().pop_back();
        }
    }
}

QJsonObject ARX::toJSON() const
{
    QJsonObject arx;
    // TODO
    throw "U idiot! not implemented yet";
    return arx;
}
