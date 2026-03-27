#ifndef PROTOCOLDEF_H
#define PROTOCOLDEF_H
enum BroadcastProtocols {
    CONNECT_REQUEST,    // Prośba o połączenie
    AUTH_MODE_SELECT,   // Wybór trybu (Kod / Bez kodu / Odrzuć)
    CODE_SEND,          // Przesłanie wygenerowanego kodu
    CODE_CHECK,         // Przesłanie wpisanego kodu
    AUTH_SUCCESS,       // Sukces autoryzacji
    AUTH_FAILED,        // Błędny kod lub odmowa
    TEST_MSG,           // Wiadomość testowa do logów
    DISCONNECT_NOTIFY   // Poinformowanie o rozłączeniu
};
#endif // PROTOCOLDEF_H
