#pragma once

#include "qjsonobject.h"
#include "utils.hpp"

class GeneratorWartosci
{
public:
    enum class TypSygnalu {
        KWADRAT = 0,
        SINUS = 1,
    };
    GeneratorWartosci(double amplituda = 1.0, double skladowaStala = 0.0, uint32_t okres = 20, TypSygnalu typSygnalu = TypSygnalu::SINUS, double wypelnienie = 0.5);


    double generuj();
    QJsonObject toJSON();
    GeneratorWartosci fromJSON(QJsonObject json);

    PROPERTY(uint32_t, Okres)
        void set(const uint32_t& value)
        {
            assert(value >= 4);
            this->value = value;
        }
    } okres;

    PROPERTY(double, Amplituda)
        void set(const double& value)
        {
            assert(value >= 0.0);
            this->value = value;
        }
    } amplituda;

    PROPERTY(double, SkladowaStala)
    } skladowaStala;

    PROPERTY(double, Wypelnienie)
        void set(const double& value)
        {
            assert(value >= 0.0);
            assert(value <= 2.0);
            this->value = value;
        }
    } wypelnienie;

    PROPERTY(TypSygnalu, TypSygnaluGeneratora)
    } typSygnalu;
    void reset();
private:
    uint32_t krok;
    inline double miejsceWCyklu() noexcept;
};
