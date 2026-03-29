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
        QHostAddress sender = dg.senderAddress();

        sender = QHostAddress(sender.toIPv4Address());

        if (dg.data() == "UAR_QUERY")
            udp->writeDatagram("UAR_RESP", sender, discoveryPort);
        else if (dg.data() == "UAR_RESP")
            emit deviceFound(sender.toString());
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

void NetService::handleNewClient(int id, QString ip)
{
    emit logAppend("Wykryto połączenie przychodzące z IP: " + ip);
}

void NetService::startAsClient(QString ip, int port)
{
    stopAll();
    client = new Client(this);

    connect(client, &Client::connected, this, [this]()
    {
        sendPacket(CONNECT_REQUEST);
    });
    connect(client, &Client::dataRecieved, this, [this](QByteArray dt) { processIncomingData(-1, dt); });
    connect(client, &Client::disconnected, this, &NetService::handleDisconnection);

    client->connectTo(ip, port);
}

void NetService::stopAll()
{
    sendPacket(DISCONNECT_NOTIFY); // Poinformowanie partnera
    stopAllLocal();
}

void NetService::stopAllLocal()
{
    if(client)
    {
        client->disconnect();
        client->deleteLater();
        client = nullptr;
    }

    if(server)
    {
        server->stopListening();
        server->deleteLater();
        server = nullptr;
    }

    authAttempts = 0;
    emit updateStatus(false, "");
}

void NetService::setAuthMode(int mode, QString code)
{
    currentAuthCode = code;
    authAttempts = 0;

    if (mode == 0)
    {
        sendPacket(CODE_SEND, currentAuthCode);
        emit logAppend("Wysłano kod autoryzacyjny do partnera.");
    }
    else if (mode == 1)
    {
        sendPacket(AUTH_SUCCESS);
        emit updateStatus(true, "Połączono (Bez kodu)");
        emit logAppend("Połączono w trybie bez autoryzacji.");
    }
}

void NetService::processIncomingData(int id, QByteArray data)
{
    QDataStream in(&data, QIODevice::ReadOnly);
    quint8 type;
    in >> type; // Odczyt typu ramki z Twojego protokołu [4]
    qDebug() << type;
    switch (type)
    {
        case CONNECT_REQUEST:
            emit authRequired("Partner");   // Wyzwala QMessageBox w ConnectionWindow
            break;

        case CODE_SEND:
        {
            QString code; in >> code;
            currentAuthCode = code;         // Zapamiętujemy kod do sprawdzenia u siebie
            emit codeEntryRequired();       // GUI: Pokaż QInputDialog do wpisania kodu
        break;
        }

        case CODE_DENY:
            emit logAppend("Odrzucono próbę połączenia.");
        break;

        case CODE_CHECK:
        {
            QString input; in >> input;     //Otrzymanie kodu
            verifyCode(input);              //Logika 3 prób
        break;
        }

        case AUTH_SUCCESS:
            emit updateStatus(true, "");
            emit logAppend("Autoryzacja udana! Tryb sieciowy aktywny.");
        break;

        case AUTH_FAILED:
        {
            QString info; in >> info;
            if (info == "FINAL")
            {
                emit logAppend("Przekroczono 3 próby. Rozłączono.");
                stopAll();
            } else emit authErrorReceived(info.toInt()); // Informacja o błędzie u wpisującego

        break;
        }

        case DISCONNECT_NOTIFY:
            emit logAppend("Partner zakończył połączenie.");
            stopAllLocal(); // Metoda czyszcząca bez wysyłania powiadomienia (uniknięcie pętli)
        break;

        case TXT_MSG:
        {
            QString msg; in >> msg;
            emit logAppend("Otrzymano: " + msg);
        break;
        }
        case SIM_DATA:
        {
            //quint32 k; double u, y;
            //in >> k >> u >> y; // Odtworzenie stanu próbki [16]
            // Przekazanie danych do warstwy prezentacji (wykresy) [18]
            //emit simulationDataReceived(u, y, k);
        break;
        }
    }
}
void NetService::sendCodeToCheck(QString code)
{
    sendPacket(CODE_CHECK, code);
}

void NetService::verifyCode(QString inputCode)
{
    if (inputCode == currentAuthCode)
    {
        sendPacket(AUTH_SUCCESS);
        emit updateStatus(true, "Zsynchronizowano"); // Sukces - NIE wywołujemy stopAll!
        emit logAppend("Autoryzacja udana!");
    }
    else
    {
        authAttempts++;
        if (authAttempts >= 3)
        {
            emit logAppend("Przekroczono limit prób!");
            sendPacket(AUTH_FAILED, "FINAL");
        } else sendPacket(AUTH_FAILED, QString::number(authAttempts));
    }
}

void NetService::sendPacket(quint8 type, QString payload)
{
    QByteArray package;
    QDataStream out(&package, QIODevice::WriteOnly);
    out << type;
    if (!payload.isEmpty()) out << payload;

    qDebug() << package;

    if (client && client->isConnected()) client->sendData(package);
    else if (server) server->sendTo(0, package); // Wysyłka do pierwszego klienta [9]
}

void NetService::handleDisconnection()
{
    emit logAppend("BŁĄD! Połączenie zerwane! Powrót do trybu stacjonarnego.");
    emit updateStatus(false, "");
}