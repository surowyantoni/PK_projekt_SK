#include "netservice.h"
#include "WarstwaUslug.h"
#include "ProtocolDef.h"
#include <QNetworkDatagram>
#include <QDataStream>
#include "DEFINITIONS.hpp"
#include "qnetworkinterface.h"
#include "qtcpsocket.h"

NetService::NetService(WarstaUslug *parent)
    : udp{QUdpSocket(this)}
    , uslugi{parent}
{
    udp.bind(CONSTS::NET::DISCOVERY_PORT, QUdpSocket::ShareAddress);
    QObject::connect(&udp, &QUdpSocket::readyRead, this, &NetService::processDiscoveryUdp);

    for (const QHostAddress &address : QNetworkInterface::allAddresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            localIP = address.toString();
}

void NetService::searchDevices()
{
    udp.writeDatagram("UAR_QUERY", QHostAddress::Broadcast, CONSTS::NET::DISCOVERY_PORT);
    emit logAppend("Wysłano zapytanie DISCOVERY...");
}

void NetService::chooseAuthWithCode(int code)
{
    assert(isServer());
    currentAuthCode = code;
    unsuccessfullAuthAttempts = 0;
    sendDataPackage(AUTH_NEEDED);
    authenticated = false;
    emit logAppend("Wysłano prośbę o wpisanie kodu do pratnera.");
}

void NetService::chooseAuthWithoutCode()
{
    assert(isServer());
    sendDataPackage(AUTH_SUCCESS);
    authenticated = true;
    unsuccessfullAuthAttempts = 0;
    emit connected();
    emit logAppend("Połączono w trybie bez autoryzacji.");
}
void  NetService::chooseAuthReject()
{
    assert(isServer());
    sendDataPackage(DISCONNECT_NOTIFY);
    emit disconnected();
    emit logAppend("Odrzucono połączenie z klientem.");
}
void NetService::authCodeVerification(int code)
{
    assert(isClient());
    sendDataPackage(AUTH_CODE, QByteArray::number(code));
    emit logAppend("Przesłano kod do weryfikacji.");
}

void NetService::startAsServer(int port)
{
    goLocal();
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &NetService::handleNewClient);
    if (server->listen(QHostAddress::Any, port))
        emit logAppend("Serwer nasłuchuje na porcie " + QString::number(port));
    else
        emit logAppend("ERROR: Port " + QString::number(port) + " wydaje się być zajęty!");
    emit uslugi->updateUI();
}

void NetService::connectAsClient(QString ip, int port)
{
    goLocal();
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, [this]()
            {
                sendDataPackage(CONNECTION_REQUEST);
            });
    connect(socket, &QTcpSocket::readyRead, this, [this]()
            {
                processDataPackage(socket->readAll());
            });
    connect(socket, &QTcpSocket::disconnected, this, [this]()
            {
                emit disconnected();
                unsuccessfullAuthAttempts = 0;
                authenticated = false;
                goLocal();
            });
    socket->connectToHost(ip, port);
    remoteIP = ip;
    emit uslugi->updateUI();
}

void NetService::disconnect()
{
    if(isAuthenticated())
    {
        sendDataPackage(DISCONNECT_NOTIFY);
        emit logAppend("Połączenie zerwane!");
    }
    unsuccessfullAuthAttempts = 0;
    authenticated = false;
    if(socket != nullptr)
        socket->disconnectFromHost();
}
void NetService::sendText(QString message)
{
    sendDataPackage(TXT_MSG, message.toUtf8());
}
void NetService::sendPidConfig()
{
    sendDataPackage(CONFIG_PID, uslugi->pid.toByteArray());
}
void NetService::sendGenConfig()
{
    sendDataPackage(CONFIG_GEN, uslugi->generator.toByteArray());
}
void NetService::sendArxConfig()
{
    sendDataPackage(CONFIG_ARX, uslugi->arx.toByteArray());
}
void NetService::sendOnOffConfig()
{
    sendDataPackage(CONFIG_ONOFF, uslugi->onOff.toByteArray());
}
void NetService::sendIntervalConfig()
{
    sendDataPackage(CONFIG_INTERVAL, uslugi->interwal.toByteArray());
}
void NetService::sendRegulationTypeConfig()
{
    sendDataPackage(CONFIG_REGULATION, uslugi->regulacja.toByteArray());
}

void NetService::sendSample(SimSampleFromRegulator sample)
{
    sendDataPackage(SIM_SAMPLE_FROM_REGULATOR, sample.toByteArray());
}
void NetService::sendSample(SimSampleFromObject sample)
{
    sendDataPackage(SIM_SAMPLE_FROM_OBJECT, sample.toByteArray());
}
void NetService::sendSimmulationRunning(bool running)
{
    if(running)
        sendDataPackage(SIM_START);
    else
        sendDataPackage(SIM_STOP);
}
void NetService::sendSimmulationRestart()
{
    sendDataPackage(SIM_RESTART);
}

void NetService::handleNewClient()
{
    while (server->hasPendingConnections())
    {
        socket = server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, [this]()
                {
                    processDataPackage(socket->readAll());
                });
        connect(socket, &QTcpSocket::disconnected, this, [this]()
                {
                    emit disconnected();
                    unsuccessfullAuthAttempts = 0;
                    authenticated = false;
                    goLocal();
                });
        remoteIP = QHostAddress(socket->peerAddress().toIPv4Address()).toString();
        emit logAppend("Wykryto połączenie przychodzące z IP: " + remoteIP);
    }
}

void NetService::goLocal()
{
    if(isServer())
    {
        server->close();
        server->deleteLater();
    }
    if(socket != nullptr)
    {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    socket = nullptr;
    server = nullptr;
}



void NetService::processDiscoveryUdp()
{
    while (udp.hasPendingDatagrams())
    {
        QNetworkDatagram dg = udp.receiveDatagram();
        QHostAddress sender = dg.senderAddress();

        sender = QHostAddress(sender.toIPv4Address());

        if (dg.data() == "UAR_QUERY")
        {
            udp.writeDatagram("UAR_RESP", sender, CONSTS::NET::DISCOVERY_PORT);
            emit deviceFound(sender.toString());
        }
        if (dg.data() == "UAR_RESP")
            emit deviceFound(sender.toString());
    }
}

void NetService::processDataPackage(QByteArray data)
{
    receivedPacketCounter++;
    QDataStream in(&data, QIODevice::ReadOnly);
    quint8 type;
    in >> type;
    QByteArray dane_pakietu;
    in >> dane_pakietu;
    switch (type)
    {
        case CONNECTION_REQUEST:
            if(isClient()) return;
            emit authChoiceQuestion();
            break;

        case AUTH_NEEDED:
            if(isServer()) return;
            emit authCodeEntryRequired();
            break;

        case AUTH_FAILED:
            if(isServer()) return;
            emit authErrorReceived();
            emit logAppend("Odrzucono próbę połączenia. Prawdopodobnie błędny kod.");
        break;

        case AUTH_CODE:
            if(isClient()) return;
            if(dane_pakietu.toInt() == currentAuthCode)
            {
                sendDataPackage(AUTH_SUCCESS);
                authenticated = true;
                emit connected();
                emit logAppend("Połączono klienta w trybie autoryzacji kodem.");
            }
            else
            {
                unsuccessfullAuthAttempts++;
                sendDataPackage(AUTH_FAILED);
                authenticated = false;
                emit logAppend("Nieudana próba połączenia z IP" + remoteIP);
            }
            if(unsuccessfullAuthAttempts >= CONSTS::NET::MAX_AUTH_ATTEMPTS)
            {
                emit logAppend("Otzymano 3 nieudane próby połączenia z IP " + remoteIP + " zamykam połączenie i czekam na nowego klienta.");
                chooseAuthReject();
            }
        break;

        case AUTH_SUCCESS:
            if(isServer()) return;
            authenticated = true;
            emit connected();
            emit logAppend("Połączenie udane! Tryb sieciowy aktywny.");
        break;

        case TXT_MSG:
            emit logAppend("Otrzymano: " + QString::fromUtf8(dane_pakietu));
            break;

        case DISCONNECT_NOTIFY:
            emit logAppend("Partner zakończył połączenie.");
            emit disconnected();
            goLocal(); // Metoda czyszcząca bez wysyłania powiadomienia (uniknięcie pętli)
            break;

        case CONFIG_PID:
            disconnectIfNotAuthenticated();
            uslugi->pid.fromByteArray(dane_pakietu);
            emit uslugi->updateUI();
            break;

        case CONFIG_ARX:
            disconnectIfNotAuthenticated();
            uslugi->arx.fromByteArray(dane_pakietu);
            emit uslugi->updateUI();
            break;

        case CONFIG_GEN:
            disconnectIfNotAuthenticated();
            uslugi->generator.fromByteArray(dane_pakietu);
            emit uslugi->updateUI();
            break;

        case CONFIG_ONOFF:
            disconnectIfNotAuthenticated();
            uslugi->onOff.fromByteArray(dane_pakietu);
            emit uslugi->updateUI();
            break;

        case CONFIG_REGULATION:
            disconnectIfNotAuthenticated();
            uslugi->regulacja.fromByteArray(dane_pakietu);
            emit uslugi->updateUI();
            break;

        case CONFIG_INTERVAL:
            disconnectIfNotAuthenticated();
            uslugi->interwal.fromByteArray(dane_pakietu);
            emit uslugi->updateUI();
            break;

        case SIM_SAMPLE_FROM_OBJECT:
            disconnectIfNotAuthenticated();
            uslugi->sampleReceivedFromARXObjectNowIHaveToBuildTheTickAndSendItToPlotsToUpdateThem(SimSampleFromObject::fromByteArray(dane_pakietu));
            // emit sampleReceivedFromObject(sample);
            break;

        case SIM_SAMPLE_FROM_REGULATOR:
            disconnectIfNotAuthenticated();
            uslugi->sampleReceivedFromREgulatorInstanceNowINeedToForewardItToTheUARAndThenSimmulateARXReactionAndSensTheSignalBack(SimSampleFromRegulator::fromByteArray(dane_pakietu));
            // emit sampleReceivedFromServer(s);
            break;

        case SIM_START:
            disconnectIfNotAuthenticated();
            uslugi->dziala.setNoSend(true);
            break;
        case SIM_STOP:
            disconnectIfNotAuthenticated();
            uslugi->dziala.setNoSend(false);
            break;
        case SIM_RESTART:
            disconnectIfNotAuthenticated();
            uslugi->reset();
            emit simmulationRestart();
            emit uslugi->updateUI();
            break;
    }
}

void NetService::sendDataPackage(quint8 type, const QByteArray &data)
{
    if(socket == nullptr)
    {
        emit disconnected();
        return;
    }
    QByteArray package;
    QDataStream out(&package, QIODevice::WriteOnly);
    out << type << data;
    if(socket->isOpen())
    {
        socket->write(package);
        socket->flush();
        transmitedPacketCounter++;
    }
    else
        emit logAppend("ERROR: Połączenie nie jest otwarte, a próbowano wysłać pakiet");
}
void NetService::disconnectIfNotAuthenticated()
{
    if(!authenticated)
    {
        disconnect();
    }
}

bool NetService::isServer()
{
    return server != nullptr;
}
bool NetService::isClient()
{
    return (socket != nullptr && server == nullptr);
}

uint32_t NetService::getTansmited()
{
    return transmitedPacketCounter;
}
uint32_t NetService::getReceived()
{
    return receivedPacketCounter;
}
bool NetService::isAuthenticated()
{
    if(socket == nullptr)
        return false;
    return authenticated;
}
