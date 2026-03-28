#include "netservice.h"
#include "ProtocolDef.h"
#include <QNetworkDatagram>


NetService::NetService(QObject *parent) : QObject(parent)
{
    udp = new QUdpSocket(this);
    udp->bind(discoveryPort, QUdpSocket::ShareAddress);
    connect(udp, &QUdpSocket::readyRead, this, &NetService::processDiscoveryUdp);
}

void NetService::searchDevices()
{
    udp->writeDatagram("UAR_QUERY", QHostAddress::Broadcast, discoveryPort);
    emit logAppend("Wysłano zapytanie DISCOVERY...");
}

void NetService::processDiscoveryUdp()
{
    while (udp->hasPendingDatagrams())
    {
        QNetworkDatagram dg = udp->receiveDatagram();
        if (dg.data() == "UAR_QUERY")
            udp->writeDatagram("UAR_RESP", dg.senderAddress(), discoveryPort);
        else if (dg.data() == "UAR_RESP")
            emit deviceFound(dg.senderAddress().toString());
    }
}

void NetService::startAsServer(int port)
{
    stopAll();
    server = new Server(this);

    connect(server, &Server::newConn, this, &NetService::handleNewClient);
    connect(server, &Server::dataReceived, this, &NetService::processIncomingData);

    if (server->listen(port))
        emit logAppend("Serwer nasłuchuje na porcie " + QString::number(port));
}

void NetService::startAsClient(QString ip, int port)
{
    stopAll();
    client = new Client(this);

    connect(client, &Client::connected, this, [this]()
    {
        QByteArray package;
        QDataStream out(&package, QIODevice::WriteOnly);
        out << (quint8)CONNECT_REQUEST;
        client->sendData(package);
    });
    connect(client, &Client::dataRecieved, this, [this](QByteArray dt) { processIncomingData(-1, dt); });
    connect(client, &Client::disconnected, this, &NetService::handleDisconnection);

    client->connectTo(ip, port);
}

void NetService::stopAll()
{
    if(client)
    {
        client->disconnect();
        delete client;
        client = nullptr;
    }

    if(server)
    {
        server->stopListening();
        delete server;
        server = nullptr;
    }
}

void NetService::processIncomingData(int id, QByteArray data)
{
    QDataStream in(&data, QIODevice::ReadOnly);
    quint8 type; in >> type;

    switch (type)
    {
        case CONNECT_REQUEST:
            emit authRequired(id == -1 ? "Partner" : "Nowy klient");
        break;
        case CODE_SEND:
            in >> currentAuthCode;
            emit codeEntryRequired();
        break;
        case CODE_CHECK:
        {
            QString input; in >> input;
            verifyCode(id, input);
        }
        break;
        case AUTH_SUCCESS:
            emit updateStatus(true, "");
        break;
    }
}

void NetService::verifyCode(int id, QString inputCode)
{
    if (inputCode == currentAuthCode)
    {
        QByteArray package;
        QDataStream out(&package, QIODevice::WriteOnly);
        out << (quint8)AUTH_SUCCESS;
        server->sendTo(id, package);
        emit updateStatus(true, "");
    }
    else
    {
        authAttempts++;
        if (authAttempts >= 3)
        {
            emit logAppend("Przekroczono 3 próby! Rozłączanie...");
            stopAll();
        }
        else
            emit logAppend("Zły kod! Próba "+ QString::number(authAttempts));
    }
}

void NetService::handleDisconnection()
{
    emit logAppend("BŁĄD! Połączenie zerwane! Powrót do trybu stacjonarnego.");
    emit updateStatus(false, "");
}