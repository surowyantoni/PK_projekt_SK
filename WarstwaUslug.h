#pragma once
#include "ARX.hpp"
#include "GeneratorWartosci.h"
#include "RegulatorOnOff.h"
#include "RegulatorPID.h"
#include "UAR.h"
#include "qtimer.h"
#include "utils.hpp"

//#include "mainwindow.h"
// #include <QObject>
// #include <QTimer>
// #include <functional>
enum WybranyRegulator { PID = 0, ONOFF = 1, nic = 2 };

class WarstaUslug : public QObject
{
    Q_OBJECT
public:
    PROPERTY(uint32_t, InterwalSymulacjiWMilisekundach)
    } interwal;

    PROPERTY(uint32_t, CzasTrwaniaSymulacjiWMilisekundach)
    private:
        void set(const uint32_t& value)
        {
            this->value = value;
        };
        void operator=(const uint32_t& value)
        {
            set(value);
        };
    } czas;

    PROPERTY_ACCESS(bool, CzySymulacjaDziala)
        void set(const bool& value)
        {
            if(value)
                static_cast<WarstaUslug*>(owner)->timer.start();
            else
                static_cast<WarstaUslug*>(owner)->timer.stop();
            this->value = value;
        }
    } dziala;
    ARX arx;
    RegulatorPID pid;
    RegulatorOnOff onOff;
    GeneratorWartosci generator;
    WarstaUslug();

    void reset();
    void uzyjRegluatora(UAR::RodzajSterowania regulacja);
signals:
    void updateCharts(UAR::Tick tick, uint32_t czas);
    void updateUI();
private:
    UAR uar;
    QTimer timer;
private slots:
    void symuluj();
    void wczytajZPliku();
    void zapiszDoPliku();
};
