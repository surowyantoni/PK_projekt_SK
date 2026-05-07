#include "ARX.hpp"
#include <QDataStream>
#include <random>

//konstruktory
ARX::ARX(std::vector<Wspolczynnik>&& wspolczynniki, int k, double z)
    : limityZadana(MinMaxClamp(CONSTS::ARX::U_min, CONSTS::ARX::U_max, CONSTS::ARX::U_active))
    , limityRegulowana(MinMaxClamp(CONSTS::ARX::Y_min, CONSTS::ARX::Y_max, CONSTS::ARX::Y_active))
    , k{k, this}
    , z{z, this}
    , U{}
    , Y{}
    , wspolczynniki{wspolczynniki, this}
{
    reset();
}

double ARX::generujZaklocenie()
{
    static std::random_device rd;
    static std::mt19937 generator(rd());

    if(z.get() == 0)
        return 0.0;

    std::normal_distribution<double> distrib(0.0, z.get());
    return distrib(generator);
}

void ARX::reset()
{
    U.clear();
    Y.clear();
    for (size_t i = 0; i < wspolczynniki.value.size() + k.get(); i++)
        U.push_back(0.0);
    for (size_t i = 0; i < wspolczynniki.value.size(); i++)
        Y.push_back(0.0);
}
double ARX::symuluj(double u)
{
    U.push_front(limityZadana.clamp(u));
    U.pop_back();

    double b = 0.0, a = 0.0;

    for (size_t i = 0; i < wspolczynniki.value.size(); i++)
    {
        b += wspolczynniki.value[i].B * U[i + k.get()]; // Mnożenie wektora B przez historię wejść U
        a += wspolczynniki.value[i].A * Y[i]; // Mnożenie wektora A przez historię wyjść Y
    }
    double y = b - a + generujZaklocenie();

    y = limityRegulowana.clamp(y);

    Y.push_front(y);
    Y.pop_back();
    return y;
}


void ARX::aktualizacjaBuforowPoZmianieOpoznienia()
{
    while(k.get() + wspolczynniki.value.size() < U.size())
    {
        U.pop_back();
    }
    while(k.get() + wspolczynniki.value.size() > U.size())
    {
        U.push_back(0.0);
    }
}

QJsonObject ARX::toJSON() const
{
    return QJsonObject();
}
void ARX::fromJSON(QJsonObject& json)
{

}
QByteArray ARX::toByteArray() const
{
    QByteArray data;
    QDataStream s(&data, QIODevice::WriteOnly);
    QByteArray wspolczynniki_arr = wspolczynniki.toByteArray();
    s << k.get() << z.get() << wspolczynniki_arr;
    return data;
}
void ARX::fromByteArray(QByteArray& data)
{
    QDataStream s(&data, QIODevice::ReadOnly);
    QByteArray wspolczynniki_arr;
    s >> k.value >> z.value >> wspolczynniki_arr;
    wspolczynniki.fromByteArray(wspolczynniki_arr);
    aktualizacjaBuforowPoZmianieOpoznienia();
}

