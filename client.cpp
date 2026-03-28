#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    connect(&socket, &QTcpSocket::connected, this, &Client::connected);
    connect(&socket, &QTcpSocket::disconnected, this, &Client::disconnected);
    connect(&socket, &QTcpSocket::readyRead, this, [this]() { emit dataRecieved(socket.readAll()); });
}

void Client::connectTo(QString ip, int port) { socket.connectToHost(ip, port); } //Inicjacja połączenia TCP

void Client::disconnect() { socket.disconnectFromHost(); } //Rozłączenie klienta

void Client::sendData(QByteArray data) { if(socket.isOpen()) socket.write(data); } //Wysyłka binarna