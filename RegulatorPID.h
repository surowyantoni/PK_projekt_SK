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


    PROPERTY_ACCESS(double, MaximumAntiWindup)
    void set(const double& value)
    {
        double min = static_cast<RegulatorPID*>(owner)->antiWindupMin;
        assert(value >= min);
        this->value = value;
    }
    } antiWindupMax;

    PROPERTY_ACCESS(double, MinimumAntiWindup)
        void set(const double& value)
        {
            double max = static_cast<RegulatorPID*>(owner)->antiWindupMax;
            assert(value <= max);
            this->value = value;
        }
    } antiWindupMin;
    PROPERTY(double, WspolczynnikAntiWindup)
    } antiWindupWspolczynnik;
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


    RegulatorPID(double k = 0.5, double Ti = 0.4, double Td = 0.1,
                double antiWindup = 0.5, double antiWindupMin = -5.0, double antiWindupMax = 5.0,
                bool antiWindupActive = true);
    PIDTick symuluj(double uchyb);
    void reset();
    void resetCzesciCalkujacej();

private:
    void zmienSposobLiczeniaCalki(SposobLiczeniaCalki value);
    double poprzedniUchyb;
    double sumaUchybowCalkowanieZewnetrzne;
    double sumaUchybowCalkowanieWewnetrzne;
};
