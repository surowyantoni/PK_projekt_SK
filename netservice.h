#ifndef NETSERVICE_H
#define NETSERVICE_H

#include "client.h"
#include "server.h"
#include <QObject>
#include <QUdpSocket>

class NetService : public QObject
{
    Q_OBJECT
public:
    explicit NetService(QObject *parent = nullptr);

    void startAsServer(int port);                               //Startuj jako serwer
    void startAsClient(QString ip, int port);                   //Startuj jako klient
    void stopAll();                                             //Zatrzymaj serwer/klient

    void searchDevices();                                       //UDP broadcast - wyszukiwanie urządzeń
    void verifyCode(QString code);                              //Weryfikacja kodu
    void setAuthMode(int mode, QString code);
    void sendCodeToCheck(QString code);
    void extracted();
    void sendPacket(quint8 type, QString payload = ""); // Wyślij pakiet

signals:                                                      // Do GUI:
    void updateStatus(bool connected, QString remoteIP = "");   //Ustaw status
    void logAppend(QString log);                                //Wypisz log

    void authRequired(QString ip);                              //Pokazuj okno akceptacji
    void authErrorReceived(int info);
    void codeEntryRequired();                                   //Wpisz kod

    void deviceFound(QString ip);                               //Znaleziono...

private slots:
    void processDiscoveryUdp();                                 //Obsługa wyszukiwania udp
    void handleNewClient(int id, QString ip);                   //Nowe połączenie
    void processIncomingData(int clientID, QByteArray data);    //Odbiór informacji
    void handleDisconnection();                                 //Rozłączenie

private:
    Client *client = nullptr;                                   //Klient
    Server *server = nullptr;                                   //Serwer
    QUdpSocket *udp = nullptr;                                  //Gniazdo UDP

    QString currentAuthCode;                                    //Aktualny kod
    const int discoveryPort = 5001;                             //Port do broadcast
    int authAttempts = 0;                                       //Próby podłączenia

    void stopAllLocal();
};

#endif // NETSERVICE_H
