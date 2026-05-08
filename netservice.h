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
signals:
    void connectionStatusChanged(bool connected, QString remoteIP = "");
    void logAppend(QString log);
    void deviceFound(QString ip);
    void disconnected();

    void authQuestionForUser();
    void authErrorReceived(QString error);
    void authCodeEntryRequired();

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

    // void sendPidConfig(QByteArray pid);
    // void sendGenConfig(QByteArray gen);
    // void sendArxConfig(QByteArray arx);
    // void sendOnOffConfig(QByteArray onoff);
    // void sendIntervalConfig(QByteArray interval);
    // void sendRegulationTypeConfig(QByteArray regtype);

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
    void handleDisconnection();
    void processDiscoveryUdp();
    void processDataPackage(QByteArray data);

private:
    void sendDataPackage(quint8 type, const QByteArray &data = QByteArray());
    // Client *client = nullptr;
    // Server *server = nullptr;
    QUdpSocket* udp = nullptr;
    QTcpSocket* socket = nullptr;
    QTcpServer* server = nullptr;
    WarstaUslug* uslugi;

    bool isServer();

    int currentAuthCode;
    int unsuccessfullAuthAttempts = 0;
    bool authenticated = false;

    int receivedPacketCounter = 0;
};

#endif // NETSERVICE_H
