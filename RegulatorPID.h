#pragma once
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
    enum SposobLiczeniaCalki { Zewnetrzne = 0, Wewnetrzne = 1 };

    MinMaxClamp limityWyjscia;
    PROPERTY(bool, AntiWindup)
    } antiWindupActive;
    PROPERTY(double, WspolczynnikiProporcjonalny)
        void set(const double& value)
        {
            assert(value >= 0.0);
            this->value = value;
        }
    } k;
    PROPERTY(double, WspolczynnikiCalkowania)
        void set(const double& value)
        {
            assert(value >= 0.0);
            this->value = value;
        }
    } Ti;
    PROPERTY(double, WspolczynnikiRozniczkowania)
        void set(const double& value)
        {
            assert(value >= 0.0);
            this->value = value;
        }
    } Td;

    PROPERTY_ACCESS(SposobLiczeniaCalki, jakLiczymyCzescCalkujaca)
        void set(const SposobLiczeniaCalki& value)
        {
            static_cast<RegulatorPID*>(owner)->zmienSposobLiczeniaCalki(value);
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
