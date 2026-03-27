#include "netservice.h"


NetService::NetService(QObject *parent) : QObject(parent)
{
    udp = new QUdpSocket(this);
    udp->bind(discoveryPort, QUdpSocket::ShareAddress);
    connect(udp, &QUdpSocket::readyRead, this, &NetService::processDiscoveryUdp);
}

void NetService::startAsServer(int port)
{
    stopAll();
    server = new Server(this);

}

void NetService::startAsClient(QString ip, int port)
{
    stopAll();
    client = new Client(this);

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
        server->stop();
        delete server;
        server = nullptr;
    }
}