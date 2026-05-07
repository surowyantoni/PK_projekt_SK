#pragma once

#include "qjsonobject.h"
#include "utils.hpp"
#include "DEFINITIONS.hpp"

class GeneratorWartosci
{
public:
    SERIALIZABLE

    enum class TypSygnalu {
        KWADRAT = 0,
        SINUS = 1,
    };
    GeneratorWartosci(double amplituda = CONSTS::Generator::amplituda,
                      double skladowaStala = CONSTS::Generator::skladowaStala,
                      uint32_t okres = secondsToMili(CONSTS::Generator::okres),
                      TypSygnalu typSygnalu = (TypSygnalu)CONSTS::Generator::sygnal,
                      double wypelnienie = CONSTS::Generator::wypelnienie);


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
