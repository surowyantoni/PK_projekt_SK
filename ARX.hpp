#pragma once

#include "DEFINITIONS.hpp"
#include "qabstractsocket.h"
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
    SERIALIZABLE

    struct Wspolczynnik
    {
        double A;
        double B;
    };
    //konstruktory
    ARX(std::vector<Wspolczynnik>&& wspolczynniki =
        {{CONSTS::ARX::A[0], CONSTS::ARX::B[0]}},
        int k = CONSTS::ARX::k,
        double z = CONSTS::ARX::z);
    MinMaxClamp limityZadana;
    MinMaxClamp limityRegulowana;

    PROP(int, ARX)
        GETTER(int)
        void set(const int &k)
        {
            this->value = k;
            owner->aktualizacjaBuforowPoZmianieOpoznienia();
        }
    } k;

    PROP(double, ARX)
        DEFAULTS(double)
    } z;

    std::deque<double> U;
    std::deque<double> Y;

    PROP(std::vector<Wspolczynnik>, ARX)
        QByteArray toByteArray() const
        {
            QByteArray data;
            QDataStream s(&data, QIODevice::WriteOnly);
            s << value.size();
            for (int idx = 0; idx < value.size(); ++idx)
            {
                s << value[idx].A << value[idx].B;
            }
            return data;
        }
        void fromByteArray(QByteArray& data)
        {
            QDataStream s(&data, QIODevice::ReadOnly);
            size_t size;
            s >> size;
            value.resize(size);
            for (int idx = 0; idx < size; ++idx)
            {
                s >> value[idx].A >> value[idx].B;
            }
        }
    } wspolczynniki;

    double symuluj(double u);
    void reset();
};
