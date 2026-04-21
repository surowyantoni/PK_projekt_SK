#include "ARX.hpp"
#include <random>

//konstruktory
ARX::ARX(std::vector<Wspolczynnik>&& wspolczynniki, int k, double z)
    : limityZadana(MinMaxClamp(-10.0, 10.0, true))
    , limityRegulowana(MinMaxClamp(-10.0, 10.0, true))
    , k(this, k)
    , z(z)
    , U(std::deque<double>())
    , Y(std::deque<double>())
    , wspolczynniki(wspolczynniki)
{
    reset();
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
    U.use().clear();
    Y.use().clear();
    for (size_t i = 0; i < wspolczynniki.get().size() + k; i++)
        U.use().push_back(0.0);
    for (size_t i = 0; i < wspolczynniki.get().size(); i++)
        Y.use().push_back(0.0);
}
double ARX::symuluj(double u)
{
    U.use().push_front(limityZadana.clamp(u));
    U.use().pop_back();

    double b = 0.0, a = 0.0;


    for (size_t i = 0; i < wspolczynniki.get().size(); i++)
    {
        b += wspolczynniki.get()[i].B * U.get()[i + k]; // Mnożenie wektora B przez historię wejść U
        a += wspolczynniki.get()[i].A * Y.get()[i]; // Mnożenie wektora A przez historię wyjść Y
    }
    double y = b - a + generujZaklocenie();

    y = limityRegulowana.clamp(y);

    Y.use().push_front(y);
    Y.use().pop_back();
    return y;
}


void ARX::aktualizacjaBuforowPoZmianieOpoznienia()
{
 // TODO
}

QJsonObject ARX::toJSON() const
{
    QJsonObject arx;
    // TODO
    throw "U idiot! not implemented yet";
    return arx;
}
