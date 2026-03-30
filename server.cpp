#include "server.h"

Server::Server(QObject *parent) : QObject(parent) {}

bool Server::listen(int port)
{
    connect(&server, &QTcpServer::newConnection, this, &Server::onNewConnection);
    return server.listen(QHostAddress::Any, port);
}

void Server::onNewConnection()
{
    while (server.hasPendingConnections())
    {
        QTcpSocket *socket = server.nextPendingConnection();
        clients.append(socket);
        int id = clients.size() - 1;

        connect(socket, &QTcpSocket::readyRead, this, [this, id]()
        {
            emit dataReceived(id, clients[id]->readAll());
        });

        QHostAddress rem_ip(socket->peerAddress().toIPv4Address());
        emit newConn(id, rem_ip.toString());
    }
}

void Server::sendTo(int id, QByteArray data) { if(id >= 0 && id < clients.size()) clients[id]->write(data); }

void Server::stopListening()
{
    server.close();
    for (auto s : clients) s->disconnectFromHost();
    clients.clear();
}