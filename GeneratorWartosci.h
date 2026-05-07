#pragma once

#include "qjsonobject.h"
#include "utils.hpp"

class GeneratorWartosci
{
public:
    SERIALIZABLE

    enum class TypSygnalu {
        KWADRAT = 0,
        SINUS = 1,
    };
    GeneratorWartosci(double amplituda = 1.0,
                      double skladowaStala = 0.0,
                      uint32_t okres = 200,
                      TypSygnalu typSygnalu = TypSygnalu::SINUS,
                      double wypelnienie = 0.5);


    double generuj(uint32_t przeskok);
    PROP(uint32_t, GeneratorWartosci)
        GETTER(uint32_t)
        void set(const uint32_t& value)
        {
            assert(value >= 1);
            this->value = value;
        }
    } okres;

    PROP(double, GeneratorWartosci)
        GETTER(double)
        void set(const double& value)
        {
            assert(value >= 0.0);
            this->value = value;
        }
    } amplituda;

    PROP(double, GeneratorWartosci)
        GETTER(double)
        SETTER(double)
    } skladowaStala;

    PROP(double, GeneratorWartosci)
        GETTER(double)
        void set(const double& value)
        {
            assert(value >= 0.0);
            assert(value <= 1.0);
            this->value = value;
        }
    } wypelnienie;

    PROP(TypSygnalu, GeneratorWartosci)
        GETTER(TypSygnalu)
        SETTER(TypSygnalu)
    } typSygnalu;
    void reset();
private:
    uint32_t krok;
    inline double miejsceWCyklu() noexcept;
};
