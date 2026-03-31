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
    void sendBinaryPacket(quint8 type, const QByteArray &data = ""); // Wyślij pakiet
    void sendSample(quint32 k, double u, double y);
    void sendPidConfig(double p, double i, double d, int mode, double min, double max);
    void sendGenConfig(int type, double amp, double period, double offset, double duty);
    void sendArxConfig(const QVector<double>& A, const QVector<double>& B, int k, double sigma, double minU, double maxU, double minY, double maxY);
    void sendSimCmd(quint8 cmd);

signals:                                                      // Do GUI:
    void updateStatus(bool connected, QString remoteIP = "");   //Ustaw status
    void logAppend(QString log);                                //Wypisz log

    void authRequired(QString ip);                              //Pokazuj okno akceptacji
    void authErrorReceived(int info);
    void codeEntryRequired();                                   //Wpisz kod

    void deviceFound(QString ip);                               //Znaleziono...

    void sampleReceived(double val, quint32 k);
    void pidUpdated(double p, double i, double d, int mode, double min, double max);
    void genUpdated(int type, double amp, double per, double off, double duty);
    void arxUpdated(const QVector<double>& A, const QVector<double>& B, int k, double sigma, double minU, double maxU, double minY, double maxY);
    void simulationResetRequested();

    void resetRequested();

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
    QString remoteIP;
    int m_packetCounter = 0;
};

#endif // NETSERVICE_H
