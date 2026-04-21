#pragma once
#include "utils.hpp"
#include <cassert>

class RegulatorOnOff
{
    enum class Stan {Off, On};
    Stan stan;
public:
    PROPERTY(double, HisterezaRegulatora)
        void set(const double& value)
        {
            assert(value > 0.0);
            if (value <= 0)
                this->value = 0.0001;
            else
                this->value = value;
        }
    } histereza;

    PROPERTY(double, WartoscSterowaniaRegulatoraUON)
    } wartoscSterowania;

    RegulatorOnOff(double wartoscSterowania = 2.0, double histereza = 0.1);
    double symuluj(double uchyb);
    void reset();
};
