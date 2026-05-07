#pragma once
#include "qjsonobject.h"
#include "utils.hpp"
#include <cassert>
#include "DEFINITIONS.hpp"

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


    RegulatorPID(double k = CONSTS::PID::P, double Ti = CONSTS::PID::I, double Td = CONSTS::PID::D,
                MinMaxClamp ograniczenia = MinMaxClamp(CONSTS::PID::U_min, CONSTS::PID::U_max, CONSTS::PID::U_active),
                bool antiWindupActive = CONSTS::PID::antiWindupActive);
    PIDTick symuluj(double uchyb);
    void reset();
    void resetCzesciCalkujacej();

private:
    void zmienSposobLiczeniaCalki(SposobLiczeniaCalki value);
    double poprzedniUchyb;
    double sumaUchybowCalkowanieZewnetrzne;
    double sumaUchybowCalkowanieWewnetrzne;
};
