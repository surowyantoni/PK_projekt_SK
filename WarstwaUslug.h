#pragma once
#include "ARX.hpp"
#include "GeneratorWartosci.h"
#include "RegulatorOnOff.h"
#include "RegulatorPID.h"
#include "UAR.h"
#include "netservice.h"
#include "qtimer.h"
#include "utils.hpp"
#include <queue>

//#include "mainwindow.h"
// #include <QObject>
// #include <QTimer>
// #include <functional>
enum WybranyRegulator { PID = 0, ONOFF = 1 };

class WarstaUslug : public QObject
{
    Q_OBJECT
public:
    using Czas = uint32_t;
    enum class TrybDzialania {
        LOCAL,
        NET_REG,
        NET_ARX,
    };

    PROP(TrybDzialania, WarstaUslug)
        GETTER(TrybDzialania)
        SETTER(TrybDzialania)
    } trybDzialania;

    PROP(uint32_t, WarstaUslug)
        GETTER(uint32_t)
        void set(const uint32_t& value)
        {
            assert(value > 1);
            owner->timer.setInterval(value);
            this->value = value;
        }
        void fromByteArray(QByteArray data)
        {
            QDataStream s(&data, QIODevice::ReadOnly);
            uint32_t interwal; s >> interwal;
            set(interwal);
        }
        QByteArray toByteArray() const
        {
            QByteArray data;
            QDataStream s(&data, QIODevice::WriteOnly);
            s << get();
            return data;
        }
    } interwal;

    PROP(bool, WarstaUslug)
        void set(const bool& value)
        {
            if(owner->trybDzialania.get() != TrybDzialania::LOCAL)
            {
                owner->netService.sendSimmulationRunning(value);
                if(owner->trybDzialania.get() == TrybDzialania::NET_ARX)
                    return;
            }
            if(value)
                owner->timer.start();
            else
                owner->timer.stop();
        }
        const bool get() const
        {
            return owner->timer.isActive();
        }
    } dziala;
    PROP(UAR::RodzajSterowania, WarstaUslug)
        void set(const UAR::RodzajSterowania& value)
        {
            switch (value)
            {
            case UAR::RodzajSterowania::OnOff:
                owner->uar.setOnOff(&owner->onOff);
            case UAR::RodzajSterowania::PID:
                owner->uar.setPID(&owner->pid);
                break;
            }
            owner->netService.sendRegulationTypeConfig();
        }
        const UAR::RodzajSterowania get() const
        {
            return owner->uar.getWybranyRegulator();
        }
        void fromByteArray(QByteArray data)
        {
            QDataStream s(&data, QIODevice::ReadOnly);
            int rodz_ster; s >> rodz_ster;
            set(static_cast<UAR::RodzajSterowania>(rodz_ster));
        }
        QByteArray toByteArray() const
        {
            QByteArray data;
            QDataStream s(&data, QIODevice::WriteOnly);
            s << get();
            return data;
        }
    } regulacja;

    WarstaUslug();

    void reset();
    ARX arx;
    RegulatorPID pid;
    RegulatorOnOff onOff;
    GeneratorWartosci generator;
    NetService netService;

public slots:
    void wczytajZPliku();
    void zapiszDoPliku();
    void sampleReceivedFromREgulatorInstanceNowINeedToForewardItToTheUARAndThenSimmulateARXReactionAndSensTheSignalBack(SimSampleFromRegulator sample); // Nazywanie funkcji to moja pasja
    void sampleReceivedFromARXObjectNowIHaveToBuildTheTickAndSendItToPlotsToUpdateThem(SimSampleFromObject sample);

signals:
    void updateCharts(UAR::Tick tick, Czas czas);
    void updateUI();

private:

    UAR uar;
    QTimer timer;
    Czas czas;

    std::queue<UAR::Tick> ticki_do_uzupelnienia;
private slots:
    void symuluj();
    };
