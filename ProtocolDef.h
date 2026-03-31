#ifndef PROTOCOLDEF_H
#define PROTOCOLDEF_H
#include "qglobal.h"
enum BroadcastProtocols
{
    CONNECT_REQUEST,    // Prośba o połączenie
    AUTH_MODE_SELECT,   // Wybór trybu (Kod / Bez kodu / Odrzuć)
    CODE_SEND,          // Przesłanie wygenerowanego kodu
    CODE_DENY,          // Odrzucenie kodu
    CODE_CHECK,         // Przesłanie wpisanego kodu
    AUTH_SUCCESS,       // Sukces autoryzacji
    AUTH_FAILED,        // Błędny kod lub odmowa
    SIM_SAMPLE,         // Próbki u, y, k (Serializacja binarna szybka)
    CONFIG_ARX,         // Współczynniki A, B, k, sigma, nasycenia (Kaskadowa)
    CONFIG_PID,         // P, I, D, tryb całki, nasycenia
    CONFIG_GEN,         // Typ, Amp, Okres, Składowa, Wypełnienie
    SIM_CMD,            // Start (1), Stop (0), Reset (2)
    TXT_MSG,            // Wiadomość tekstowa do logów
    DISCONNECT_NOTIFY   // Poinformowanie o rozłączeniu
};

#pragma pack(push, 1)
struct SimSample
{
    quint32 k;    // Identyfikator kolejności próbki
    double u;
    double y;    // Przesyłana wartość (u od klienta, y od serwera)

};
#pragma pack(pop)

#endif // PROTOCOLDEF_H
