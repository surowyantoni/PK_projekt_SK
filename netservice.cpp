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
    remoteIP = ip;
    emit logAppend("Wykryto połączenie przychodzące z IP: " + ip);
}

void NetService::startAsClient(QString ip, int port)
{
    stopAll();
    client = new Client(this);

    connect(client, &Client::connected, this, [this]()
    {
        sendBinaryPacket(CONNECT_REQUEST);
    });
    connect(client, &Client::dataRecieved, this, [this](QByteArray dt) { processIncomingData(-1, dt); });
    connect(client, &Client::disconnected, this, &NetService::handleDisconnection);

    remoteIP = ip;
    client->connectTo(ip, port);
}

void NetService::stopAll()
{
    sendBinaryPacket(DISCONNECT_NOTIFY); // Poinformowanie partnera
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
        sendBinaryPacket(CODE_SEND, currentAuthCode.toUtf8());
        emit logAppend("Wysłano kod autoryzacyjny do partnera.");
    }
    else if (mode == 1)
    {
        sendBinaryPacket(AUTH_SUCCESS);
        emit updateStatus(true, remoteIP);
        emit logAppend("Połączono w trybie bez autoryzacji.");
    }
}

void NetService::processIncomingData(int id, QByteArray data)
{
    QDataStream in(&data, QIODevice::ReadOnly);
    quint8 type;
    in >> type;                             //Odczyt typu z protokołu
    qDebug() << type;
    switch (type)
    {
        case CONNECT_REQUEST:
            emit authRequired("Partner");   // Wyzwala QMessageBox w ConnectionWindow
            break;

        case CODE_SEND:
        {
            QByteArray data; in >> data;
            QString code = QString::fromUtf8(data);
            currentAuthCode = code;         // Zapamiętujemy kod do sprawdzenia u siebie
            emit codeEntryRequired();       // GUI: Pokaż QInputDialog do wpisania kodu
        break;
        }

        case CODE_DENY:
            emit logAppend("Odrzucono próbę połączenia.");
        break;

        case CODE_CHECK:
        {
            QByteArray data; in >> data;

            QString input = QString::fromUtf8(data);    //Otrzymanie kodu
            verifyCode(input);                          //Logika 3 prób
        break;
        }

        case AUTH_SUCCESS:
            emit updateStatus(true, remoteIP);
            emit logAppend("Autoryzacja udana! Tryb sieciowy aktywny.");
        break;

        case AUTH_FAILED:
        {
            QByteArray data; in >> data;
            QString info = QString::fromUtf8(data);
            if (info == "FINAL")
            {
                emit logAppend("Przekroczono 3 próby. Rozłączono.");
                stopAll();
            } else emit authErrorReceived(info.toInt()); // Informacja o błędzie u wpisującego

        break;
        }

        case CONFIG_PID:
        {
            m_packetCounter++;
            double p, i, d, min, max; int mode;
            in >> p >> i >> d >> mode >> min >> max;
            emit pidUpdated(p, i, d, mode, min, max); // GUI aktualizuje kontrolki [10]
            break;
        }
        case CONFIG_GEN:
        {
            m_packetCounter++;
            int type; double amp, per, off, duty;
            in >> type >> amp >> per >> off >> duty;
            emit genUpdated(type, amp, per, off, duty);
            break;
        }
        case SIM_SAMPLE:
        {
            m_packetCounter++;
            SimSample s;
            memcpy(&s, data.data() + 1, sizeof(SimSample)); // Deserializacja szybka 1:1 [11]
            emit sampleReceived(s.u, s.k);
            break;
        }

        case CONFIG_ARX:
        {
            m_packetCounter++;
            QVector<double> A, B; int k; double sigma, minU, maxU, minY, maxY;
            in >> A >> B >> k >> sigma >> minU >> maxU >> minY >> maxY;
            emit arxUpdated(A, B, k, sigma, minU, maxU, minY, maxY);
            break;
        }
        case SIM_CMD:
        {
            m_packetCounter++;
            quint8 cmd; in >> cmd; // 1-Start, 0-Stop, 2-Reset
            if(cmd == 2)
                // Reset zeruje bufory, ale NIE ustawienia [12]
                emit resetRequested();
            break;
        }

        case TXT_MSG:
        {
            m_packetCounter++;
            QByteArray data; in >> data;

            QString msg = QString::fromUtf8(data);
            emit logAppend("Otrzymano: " + msg);
            break;
        }

        case DISCONNECT_NOTIFY:
            emit logAppend("Partner zakończył połączenie.");
            stopAll(); // Metoda czyszcząca bez wysyłania powiadomienia (uniknięcie pętli)
        break;
    }
}
void NetService::sendCodeToCheck(QString code)
{
    sendBinaryPacket(CODE_CHECK, code.toUtf8());
}

void NetService::verifyCode(QString inputCode)
{
    if (inputCode == currentAuthCode)
    {
        sendBinaryPacket(AUTH_SUCCESS);
        emit updateStatus(true, remoteIP); // Sukces - NIE wywołujemy stopAll!
        emit logAppend("Autoryzacja udana!");
    }
    else
    {
        authAttempts++;
        if (authAttempts >= 3)
        {
            emit logAppend("Przekroczono limit prób!");
            sendBinaryPacket(AUTH_FAILED, "FINAL");
        } else sendBinaryPacket(AUTH_FAILED, QString::number(authAttempts).toUtf8());
    }
}


void NetService::sendBinaryPacket(quint8 type, const QByteArray &data) {
    QByteArray package;
    QDataStream out(&package, QIODevice::WriteOnly);
    out << type << data;

    if (client && client->isConnected()) client->sendData(package);
    else if (server) server->sendTo(0, package);
}

void NetService::sendSample(quint32 k, double u, double y)
{
    SimSample  packet = { k, u, y };
    QByteArray data;
    data.append((char)SIM_SAMPLE);
    data.append(reinterpret_cast<const char*>(&packet), sizeof(SimSample));

    if (client && client->isConnected()) client->sendData(data);
    else if (server) server->sendTo(0, data);
}

// SYNCHRONIZACJA PID
void NetService::sendPidConfig(double p, double i, double d, int mode, double min, double max)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << p << i << d << mode << min << max;
    sendBinaryPacket(CONFIG_PID, data);
    QString bitString;
    for (char byte : data) {
        // Convert each byte to an unsigned integer
        unsigned char ubyte = static_cast<unsigned char>(byte);
        // Loop through each bit (MSB first)
        for (int i = 7; i >= 0; --i) {
            bitString.append(((ubyte >> i) & 1) ? '1' : '0');
        }
    }
    qDebug() << bitString;
    qDebug() << "P: " << p << "I: " << i << "D: " << d;

    QDataStream in(&data, QIODevice::ReadOnly);
}

// SYNCHRONIZACJA GENERATORA
void NetService::sendGenConfig(int type, double amp, double period, double offset, double duty)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << type << amp << period << offset << duty;
    sendBinaryPacket(CONFIG_GEN, data);
}

//SYNCHRONIZACJA ARX
void NetService::sendArxConfig(const QVector<double>& A, const QVector<double>& B, int k, double sigma, double minU, double maxU, double minY, double maxY) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << A << B << k << sigma << minU << maxU << minY << maxY; // QDataStream sam dodaje rozmiar QVector
    sendBinaryPacket(CONFIG_ARX, data);
}

void NetService::handleDisconnection()
{
    emit logAppend("BŁĄD! Połączenie zerwane! Powrót do trybu stacjonarnego.");
    emit updateStatus(false, "");
}
