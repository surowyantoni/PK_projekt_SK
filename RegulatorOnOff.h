#pragma once
#include "qjsonobject.h"
#include "utils.hpp"
#include <cassert>

class RegulatorOnOff
{
    enum class Stan {Off, On};
    Stan stan;
public:
    SERIALIZABLE
    PROP(double, RegulatorOnOff)
        GETTER(double)
        void set(const double& value)
        {
            assert(value > 0.0);
            if (value <= 0)
                this->value = 0.0001;
            else
                this->value = value;
        }
    } histereza;

    PROP(double, RegulatorOnOff)
        GETTER(double)
        SETTER(double)
    } wartoscSterowania;

    RegulatorOnOff(double wartoscSterowania = 2.0, double histereza = 0.1);
    double symuluj(double uchyb);
    void reset();
};
