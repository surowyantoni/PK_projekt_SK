#include "netservice.h"
#include "WarstwaUslug.h"
#include "ProtocolDef.h"
#include <QNetworkDatagram>
#include <QDataStream>
#include "DEFINITIONS.hpp"
#include "qtcpsocket.h"

NetService::NetService(WarstaUslug *parent)
    : udp{new QUdpSocket(this)}
    , uslugi{parent}
{
    udp->bind(CONSTS::NET::DISCOVERY_PORT, QUdpSocket::ShareAddress);
    connect(udp, &QUdpSocket::readyRead, this, &NetService::processDiscoveryUdp);
}

void NetService::searchDevices()
{
    udp->writeDatagram("UAR_QUERY", QHostAddress::Broadcast, CONSTS::NET::DISCOVERY_PORT);
    emit logAppend("Wysłano zapytanie DISCOVERY...");
}

void NetService::chooseAuthWithCode(int code)
{
    currentAuthCode = code;
    unsuccessfullAuthAttempts = 0;
    authenticated = false;
    sendDataPackage(AUTH_NEEDED);
    emit logAppend("Wysłano prośbę o wpisanie kodu do pratnera.");
}

void NetService::chooseAuthWithoutCode()
{
    sendDataPackage(AUTH_SUCCESS);
    authenticated = true;
    emit logAppend("Połączono w trybie bez autoryzacji.");
}
void  NetService::chooseAuthReject()
{
    sendDataPackage(DISCONNECT_NOTIFY);
    emit logAppend("Odrzucono połączenie z klientem");
}
void NetService::authCodeVerification(int code)
{
    sendDataPackage(AUTH_CODE, QByteArray::number(code));
}

void NetService::startAsServer(int port)
{
    startLocal();
    server = new QTcpServer(this);

    connect(server, &QTcpServer::newConnection, this, &NetService::handleNewClient);
    if (server->listen(QHostAddress::Any, port))
        emit logAppend("Serwer nasłuchuje na porcie " + QString::number(port));
    emit uslugi->updateUI();
}

void NetService::connectAsClient(QString ip, int port)
{
    startLocal();
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, [this]()
            {
                sendDataPackage(CONNECTION_REQUEST);
            });
    connect(socket, &QTcpSocket::readyRead, this, [this]()
            {
                processDataPackage(socket->readAll());
            });
    connect(socket, &QTcpSocket::disconnected, this, &NetService::startLocal);
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
    startLocal();
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
        connect(socket, &QTcpSocket::disconnected, this, &NetService::startLocal);
        remoteIP = QHostAddress(socket->peerAddress().toIPv4Address()).toString();
        emit logAppend("Wykryto połączenie przychodzące z IP: " + remoteIP);
    }
}

void NetService::startLocal()
{
    if(isServer())
    {
        server->close();
        server->deleteLater();
    }
    if(isAuthenticated())
    {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    socket = nullptr;
    server = nullptr;

    unsuccessfullAuthAttempts = 0;
    authenticated = false;
    emit connectionStatusChanged(true, remoteIP);
    emit uslugi->updateUI();
}



void NetService::processDiscoveryUdp()
{
    while (udp->hasPendingDatagrams())
    {
        QNetworkDatagram dg = udp->receiveDatagram();
        QHostAddress sender = dg.senderAddress();

        sender = QHostAddress(sender.toIPv4Address());

        if (dg.data() == "UAR_QUERY")
            udp->writeDatagram("UAR_RESP", sender, CONSTS::NET::DISCOVERY_PORT);
        else if (dg.data() == "UAR_RESP")
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
            if(!isServer()) return;
            emit authQuestionForUser();
            break;

        case AUTH_NEEDED:
            if(isServer()) return;
            emit authCodeEntryRequired();
            break;

        case AUTH_FAILED:
            if(isServer()) return;
            emit authCodeEntryRequired();
            emit authErrorReceived("Podałeś błędny kod");
            emit logAppend("Odrzucono próbę połączenia. Prawdopodobnie błędny kod.");
        break;

        case AUTH_CODE:
            if(!isServer()) return;
            if(dane_pakietu.toInt() == currentAuthCode)
            {
                sendDataPackage(AUTH_SUCCESS);
                authenticated = true;
                emit connectionStatusChanged(true, remoteIP);
                emit logAppend("Połączono klienta w trybie autoryzacji kodem.");
            }
            else
            {
                unsuccessfullAuthAttempts++;
                authenticated = false;
                emit logAppend("Nieudana próba połączenia z IP" + remoteIP);
                sendDataPackage(AUTH_FAILED);
            }
            if(unsuccessfullAuthAttempts >= CONSTS::NET::MAX_AUTH_ATTEMPTS)
            {
                emit logAppend("Otzymano 3 takie nieudane próby, zamykam połączenie z " + remoteIP + " i czekam na nowego klienta.");
                //Restartuję serwer, bo to najłatwiejsze
                startAsServer(server->serverPort());
            }
        break;

        case AUTH_SUCCESS:
            if(isServer()) return;
            authenticated = true;
            emit connectionStatusChanged(true, remoteIP);
            emit logAppend("Połączenie udane! Tryb sieciowy aktywny.");
        break;

        case TXT_MSG:
            emit logAppend("Otrzymano: " + QString::fromUtf8(dane_pakietu));
            break;

        case DISCONNECT_NOTIFY:
            emit logAppend("Partner zakończył połączenie.");
            startLocal(); // Metoda czyszcząca bez wysyłania powiadomienia (uniknięcie pętli)
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
            uslugi->dziala.set(true);
            emit uslugi->updateUI();
            break;
        case SIM_STOP:
            disconnectIfNotAuthenticated();
            uslugi->dziala.set(false);
            emit uslugi->updateUI();
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
    QByteArray package;
    QDataStream out(&package, QIODevice::WriteOnly);
    out << type << data;
    assert(isAuthenticated());
    if(socket->isOpen())
    {
        socket->write(package);
        socket->flush();
        transmitedPacketCounter++;
    }
    else
        qDebug() << "[ERR] Połączenie nie jest otwarte";
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
bool NetService::isAuthenticated()
{
    if(socket == nullptr)
        return false;
    return authenticated;
}
