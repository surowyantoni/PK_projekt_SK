#pragma once
#include "qjsonobject.h"
#include "utils.hpp"
#include <cassert>
#include "DEFINITIONS.hpp"

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

    RegulatorOnOff(double wartoscSterowania = CONSTS::OnOff::sterowanie, double histereza = CONSTS::OnOff::histereza);
    double symuluj(double uchyb);
    void reset();
};
