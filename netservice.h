#ifndef NETSERVICE_H
#define NETSERVICE_H

#include <QObject>
#include <QUdpSocket>
#include "ProtocolDef.h"
#include "qtcpserver.h"

class WarstaUslug;

class NetService : public QObject
{
    Q_OBJECT
public:
    explicit NetService(WarstaUslug* parent = nullptr);
    QString remoteIP = "";
    QString localIP = "";

    bool isServer();
    bool isClient();
    bool isAuthenticated();

    uint32_t getTansmited();
    uint32_t getReceived();
signals:
    void logAppend(QString log);
    void deviceFound(QString ip);
    void netError(QString errorMsg);

    void authChoiceQuestion();

    void authCodeEntryRequired();
    void authErrorReceived();
    void connected();

    void disconnected();
    void simmulationRestart();
    // void simmulationStart();
    // void simmulationStop();
    // void sampleReceivedFromClient(SimSampleFromObject sample);
    // void sampleReceivedFromServer(SimSampleFromRegulator sample);


public slots:
    void searchDevices();
    void chooseAuthWithCode(int code);
    void chooseAuthWithoutCode();
    void chooseAuthReject();

    void authCodeVerification(int code);

    void startAsServer(int port);
    void connectAsClient(QString ip, int port);
    void disconnect();
    void goLocal();


    void sendText(QString message);

    void sendPidConfig();
    void sendGenConfig();
    void sendArxConfig();
    void sendOnOffConfig();
    void sendIntervalConfig();
    void sendRegulationTypeConfig();

    void sendSample(SimSampleFromRegulator sample);
    void sendSample(SimSampleFromObject sample);
    void sendSimmulationRestart();
    void sendSimmulationRunning(bool running);

private slots:
    void handleNewClient();
    void processDiscoveryUdp();
    void processDataPackage(QByteArray data);

private:
    void disconnectIfNotAuthenticated();
    void sendDataPackage(quint8 type, const QByteArray &data = QByteArray());

    QUdpSocket udp;
    QTcpSocket* socket = nullptr;
    QTcpServer* server = nullptr;
    WarstaUslug* uslugi;


    int currentAuthCode = 0000;
    int unsuccessfullAuthAttempts = 0;
    bool authenticated = false;

    int receivedPacketCounter = 0;
    int transmitedPacketCounter = 0;
};

#endif // NETSERVICE_H
