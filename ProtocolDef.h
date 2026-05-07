#ifndef PROTOCOLDEF_H
#define PROTOCOLDEF_H

#include "qabstractsocket.h"
#include "qglobal.h"
#include "qobject.h"
#include "qstringview.h"
enum ProtocolActions
{
    CONNECTION_REQUEST, // Prośba o połączenie
    AUTH_NEEDED,        // Prośba o wpisanie kodu
    AUTH_CODE,          // Przesłanie wpisanego kodu
    AUTH_SUCCESS,       // Sukces autoryzacji i nawiązanie połączenia
    AUTH_FAILED,        // Błędny kod lub odmowa
    TXT_MSG,            // Wiadomość tekstowa do logów
    DISCONNECT_NOTIFY,   // Poinformowanie o rozłączeniu
    CONFIG_ARX,         // Współczynniki A, B, k, sigma, nasycenia (Kaskadowa)
    CONFIG_PID,         // P, I, D, tryb całki, nasycenia
    CONFIG_GEN,         // Typ, Amp, Okres, Składowa, Wypełnienie
    CONFIG_ONOFF,
    CONFIG_REGULATION,
    CONFIG_INTERVAL,
    SIM_SAMPLE_FROM_OBJECT,         // Próbki odebrane od obiektu
    SIM_SAMPLE_FROM_REGULATOR,      // Próbki odebrane od regulatora
    SIM_RESTART,
    SIM_START,
    SIM_STOP,
};

#pragma pack(push, 1)
struct SimSampleFromRegulator
{
    double wartoscZadana; // Wyjscie z generatora
    double sterowanie;    // Wyjście z regulatora


    static SimSampleFromRegulator fromByteArray(QByteArray data)
    {
        SimSampleFromRegulator sample;
        QDataStream s(&data, QIODevice::ReadOnly);
        s >> sample.wartoscZadana >> sample.sterowanie;
        return sample;
    }
    QByteArray toByteArray() const
    {
        QByteArray data;
        QDataStream s(&data, QIODevice::WriteOnly);
        s << wartoscZadana << sterowanie;
        return data;
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SimSampleFromObject
{
    double wartoscRegulowana; // Wyjscie z obiektu

    static SimSampleFromObject fromByteArray(QByteArray data)
    {
        SimSampleFromObject sample;
        QDataStream s(&data, QIODevice::ReadOnly);
        s >> sample.wartoscRegulowana;
        return sample;
    }
    QByteArray toByteArray() const
    {
        QByteArray data;
        QDataStream s(&data, QIODevice::WriteOnly);
        s << wartoscRegulowana;
        return data;
    }

};
#pragma pack(pop)

#endif // PROTOCOLDEF_H
