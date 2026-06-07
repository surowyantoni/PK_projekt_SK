#pragma once
#include "ARX.hpp"
#include "GeneratorWartosci.h"
#include "RegulatorOnOff.h"
#include "RegulatorPID.h"
#include "UAR.h"
#include "netservice.h"
#include "qtimer.h"
#include "utils.hpp"
#include <QElapsedTimer>
#include <QMessageBox>

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
        NET_ARX_OWN_TICK,
    };

    PROP(TrybDzialania, WarstaUslug)
        GETTER(TrybDzialania)
        void set(TrybDzialania tryb)
        {
            if(this->value == tryb)
                return;
            if(owner->authenticated)
            {
                if (QMessageBox::question(nullptr, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?") == QMessageBox::Yes)
                {
                    owner->disconnectGracefully();
                }
                else
                {
                    emit owner->updateUI();
                    return; // brak zmiany wartości
                }
            }
            this->value = tryb;
            emit owner->updateUI();
        }
        bool isLocal()
        {
            return this->value == WarstaUslug::TrybDzialania::LOCAL;
        }
        bool isSimmulationRegulator()
        {
            return this->value == WarstaUslug::TrybDzialania::NET_REG;
        }
        bool isSimmulationObject()
        {
            return this->value == WarstaUslug::TrybDzialania::NET_ARX ||
                this->value == WarstaUslug::TrybDzialania::NET_ARX_OWN_TICK;
        }
    } trybDzialania;

    PROP(uint32_t, WarstaUslug)
        GETTER(uint32_t)
        void set(const uint32_t& value)
        {
            assert(value > 1);
            owner->timer.setInterval(value);
            this->value = value;
            if(owner->trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
                owner->sendDataPackage(CONFIG_INTERVAL, this->toByteArray());
            emit owner->updateUI();
        }
        void fromByteArray(QByteArray data)
        {
            QDataStream s(&data, QIODevice::ReadOnly);
            uint32_t interwal; s >> interwal;
            owner->timer.setInterval(interwal);
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
            if(value)
                owner->timer.start();
            else
                owner->timer.stop();
            emit owner->updateUI();
        }
        const bool get() const
        {
            return owner->timer.isActive();
        }
    } dziala;

    PROP(UAR::RodzajSterowania, WarstaUslug)
        void set(const UAR::RodzajSterowania& value)
        {
            owner->uar.regulator = value;
            if(owner->authenticated)
                owner->sendDataPackage(CONFIG_REGULATION, this->toByteArray());
            emit owner->updateUI();
        }
        const UAR::RodzajSterowania get() const
        {
            return owner->uar.regulator;
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

    PROP(QString, WarstaUslug)
        GETTER(QString)
    } remoteIP;
    PROP(QString, WarstaUslug)
        GETTER(QString)
    } localIP;

    PROP(uint32_t, WarstaUslug)
        GETTER(uint32_t)
    } tansmitedPackets;

    PROP(uint32_t, WarstaUslug)
        GETTER(uint32_t)
    } receivedPackets;

    WarstaUslug();
    void reset();

    PROP(qint64, WarstaUslug)
        GETTER(qint64);
    } measuredInterval;

    PROP(bool, WarstaUslug)
    GETTER(bool);
    } authenticated;


    // Klasy senderów, po to żeby po zmianie paramatru
    // automatycznie przesyłała się konfiguracja po sieci


    AccessNotifier<ARX> arxChange()
    {
        return AccessNotifier<ARX>(&arx, [this]()
        {
            if(authenticated) sendDataPackage(CONFIG_ARX, arx.toByteArray());
            emit updateUI();
        });
    }

    AccessNotifier<RegulatorPID> pidChange()
    {
        return AccessNotifier<RegulatorPID>(&pid, [this]()
        {
            if(authenticated) sendDataPackage(CONFIG_PID, pid.toByteArray());
            emit updateUI();
        });
    }
    AccessNotifier<RegulatorOnOff> onOffChange()
    {
        return AccessNotifier<RegulatorOnOff>(&onOff, [this]()
        {
            if(authenticated) sendDataPackage(CONFIG_ONOFF, onOff.toByteArray());
            emit updateUI();
        });
    }
    AccessNotifier<GeneratorWartosci> generatorChange()
    {
        return AccessNotifier<GeneratorWartosci>(&generator, [this]()
        {
            if(authenticated) sendDataPackage(CONFIG_GEN, generator.toByteArray());
            emit updateUI();
        });
    }

    ARX arx;
    RegulatorPID pid;
    RegulatorOnOff onOff;
    GeneratorWartosci generator;


    bool wczytajZPliku(QString plik);
    void zapiszDoPliku(QString plik);

    void searchDevices();
    void chooseAuthWithCode(int code);
    void chooseAuthWithoutCode();
    void chooseAuthReject();

    void authCodeVerification(int code);

    void startAsServer(int port);
    void connectAsClient(QString ip, int port);
    void disconnectGracefully();

    void sendText(QString message);

    void sendSample(SimSampleFromRegulator sample);
    void sendSample(SimSampleFromObject sample);
    void sendSimmulationRestart();
    void sendSimmulationRunning(bool running);

public slots:
    void sampleReceivedFromRegulatorInstanceNowINeedToForewardItToTheUARAndThenSimmulateARXReactionAndSensTheSignalBack(SimSampleFromRegulator sample); // Nazywanie funkcji to moja pasja
    void sampleReceivedFromARXObject(SimSampleFromObject sample);

signals:
    void updateCharts(UAR::Tick tick, Czas czas);
    void updateUI();

    void symulacjaWyrabiaSie(bool wyrabia);

    void netLogAppend(QString log);
    void netDeviceFound(QString ip);
    void netError(QString errorMsg);

    void authChoiceQuestion();
    void authCodeEntryRequired();
    void authErrorReceived();
    void connected();
    void disconnected();
    void simmulationRestart(); // możliwe że do usuniecia

private:



    bool isServerStarted();
    bool isClientStarted();

    UAR uar;
    QTimer timer;
    Czas czas;
    QElapsedTimer elapsed;

    void handleUnexpecteadDisconnection();
    void disconnectIfNotAuthenticated();
    void sendDataPackage(quint8 type, const QByteArray &data = QByteArray());

    QUdpSocket udpDiscovery;
    QUdpSocket udpForSamples;
    uint port;
    QTcpSocket* socket = nullptr;
    QTcpServer* server = nullptr;


    int currentAuthCode;
    int unsuccessfullAuthAttempts;
    bool authenticated;

private slots:
    void symuluj();

    void handleNewClient();
    void processDiscoveryUdp();
    void processDataPackage(QByteArray data);
    };
