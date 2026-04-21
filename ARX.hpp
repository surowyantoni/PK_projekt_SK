#pragma once

#include "utils.hpp"
#include "qjsonobject.h"
#include <deque>
#include <vector>


class ARX
{
private:
    void aktualizacjaBuforowPoZmianieOpoznienia();
    double generujZaklocenie();
public:
    struct Wspolczynnik
    {
        double A;
        double B;
    };
    //konstruktory
    ARX(std::vector<Wspolczynnik>&& wspolczynniki = {{-0.4, 0.6}},
        int k = 1,
        double z = 0.0);
    MinMaxClamp limityZadana;
    MinMaxClamp limityRegulowana;

    PROPERTY_ACCESS(int, OpoznienieTransportowe)
        void set(const int &k)
        {
            value = k;
            static_cast<ARX*>(owner)->aktualizacjaBuforowPoZmianieOpoznienia();
        }
    } k;



    PROPERTY(double, Zaklocenia)
    } z;

    PROPERTY(std::deque<double>, Wejscie)
    } U;
    PROPERTY(std::deque<double>, Wyjscie)
    } Y;

    PROPERTY(std::vector<Wspolczynnik>, WspolczynnikiObiektuARX)
    } wspolczynniki;

    double symuluj(double u);
    void reset();

    QJsonObject toJSON() const;
    static ARX fromJSON(QJsonObject& json);
};
