#pragma once
#include "qjsonobject.h"
#include "utils.hpp"
#include <cassert>

struct PIDTick
{
    double P;
    double I;
    double D;

    operator double() const noexcept;
};

class RegulatorPID
{
public:
    SERIALIZABLE
    enum SposobLiczeniaCalki { Zewnetrzne = 0, Wewnetrzne = 1 };

    MinMaxClamp limityWyjscia;
    PROP(bool, RegulatorPID)
        GETTER(bool)
        SETTER(bool)
    } antiWindupActive;
    PROP(double, RegulatorPID)
        GETTER(double)
        void set(const double& value)
        {
            assert(value >= 0.0);
            this->value = value;
        }
    } k;
    PROP(double, RegulatorPID)
        GETTER(double)
        void set(const double& value)
        {
            assert(value >= 0.0);
            this->value = value;
        }
    } Ti;
    PROP(double, RegulatorPID)
        GETTER(double)
        void set(const double& value)
        {
            assert(value >= 0.0);
            this->value = value;
        }
    } Td;

    PROP(SposobLiczeniaCalki, RegulatorPID)
        GETTER(SposobLiczeniaCalki)
        void set(const SposobLiczeniaCalki& value)
        {
            owner->zmienSposobLiczeniaCalki(value);
            this->value = value;
        }
    } sposobLiczeniaCalki;


    RegulatorPID(double k = 0.5, double Ti = 5.0, double Td = 0.0,
                 MinMaxClamp ograniczenia = MinMaxClamp(-100.0, 100.0, true), bool antiWindupActive = true);
    PIDTick symuluj(double uchyb);
    void reset();
    void resetCzesciCalkujacej();

private:
    void zmienSposobLiczeniaCalki(SposobLiczeniaCalki value);
    double poprzedniUchyb;
    double sumaUchybowCalkowanieZewnetrzne;
    double sumaUchybowCalkowanieWewnetrzne;
};
