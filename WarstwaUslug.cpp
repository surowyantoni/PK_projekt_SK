#include "WarstwaUslug.h"
#include "qdir.h"
#include "qnetworkdatagram.h"
#include "qnetworkinterface.h"
#include "qtcpsocket.h"
#include <QMessageBox>
#include <QJsonDocument>


WarstaUslug::WarstaUslug()
    : trybDzialania{TrybDzialania::LOCAL, this}
    , interwal{CONSTS::UAR::interwal, this}
    , dziala{CONSTS::UAR::started, this}
    , regulacja{(UAR::RodzajSterowania)CONSTS::UAR::regulator, this}
    , remoteIP{"", this}
    , localIP{"", this}
    , tansmitedPackets{0, this}
    , receivedPackets{0, this}
    , measuredInterval{0, this}
    , authenticated{false, this}
    , arx(ARX())
    , pid(RegulatorPID())
    , onOff(RegulatorOnOff())
    , generator(GeneratorWartosci())
    , uar(UAR(&arx, &generator, &pid, &onOff, (UAR::RodzajSterowania)CONSTS::UAR::regulator))
    , timer(this)
    , czas{0}
    , elapsed()
    , oczekujacy_tick{std::nullopt}
    , ostatnia_rzeczywista_wartosc_z_obiektu_arx{0.0}
    , udpDiscovery{QUdpSocket(this)}
    , udpForSamples{QUdpSocket(this)}
    , port{0}
    , currentAuthCode{0000}
    , unsuccessfullAuthAttempts(0)
{
    timer.setTimerType(Qt::PreciseTimer); // żeby działał dokładniej
    dziala.set(dziala.get()); // wywoalanie settera - ustawienie timera
    interwal.set(interwal.get()); // wywoalanie settera - ustawienie timera
    QObject::connect(&timer, &QTimer::timeout, this, &WarstaUslug::symuluj);
    elapsed.start();

    udpDiscovery.bind(CONSTS::NET::DISCOVERY_PORT, QUdpSocket::ShareAddress);
    QObject::connect(&udpDiscovery, &QUdpSocket::readyRead, this, &WarstaUslug::processDiscoveryUdp);

    for (const QHostAddress &address : QNetworkInterface::allAddresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            localIP.value = address.toString();

}

void WarstaUslug::reset(bool send_over_network)
{
    arx.reset();
    pid.reset();
    onOff.reset();
    generator.reset();
    czas = 0;
    if(send_over_network && authenticated.get())
        sendDataPackage(SIM_RESTART);
}

void WarstaUslug::symuluj()
{
    czas = interwal.get() + czas;
    measuredInterval.value = elapsed.elapsed();
    elapsed.start();
    switch (trybDzialania.get())
    {
    case TrybDzialania::LOCAL:
        emit updateCharts(uar.symuluj(interwal.get()), czas);
        emit symulacjaWyrabiaSie(measuredInterval.get() < interwal.get() + CONSTS::GUI::MAX_RUN_LAG_BEFRE_MARKING_RED);
        break;
    case TrybDzialania::NET_REG:
        if(oczekujacy_tick.extrapolated)
        {
            uar.zaktualizujPoprzendieWyjscie(ostatnia_rzeczywista_wartosc_z_obiektu_arx);
            oczekujacy_tick.wartoscRegulowana = ostatnia_rzeczywista_wartosc_z_obiektu_arx;
            emit updateCharts(oczekujacy_tick, czas);
            emit symulacjaWyrabiaSie(false);
        }
        oczekujacy_tick = uar.symulujRegulator(interwal.get());
        oczekujacy_tick.extrapolated = true;
        sendSample(SimSampleFromRegulator {
          .wartoscZadana = oczekujacy_tick.wartoscZadana,
          .sterowanie = oczekujacy_tick.sterowanie,
          .czas = czas
        });
        break;
    case TrybDzialania::NET_ARX:
        timer.stop();
        czas -= interwal.get();
        break;
    case TrybDzialania::NET_ARX_OWN_TICK:
        break;
    }
}


bool WarstaUslug::wczytajZPliku(QString plik)
{
    //TODO
    QFile file(plik);
    try
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throw 1;

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject root = doc.object();

        QJsonObject arx_obj = root["arx"].toObject();
        arx.fromJSON(arx_obj);
    }
    catch (...)
    {
        return false;
    }
    file.close();
    emit updateUI();
    return true;
}
void WarstaUslug::zapiszDoPliku(QString plik)
{
    //TODO
    QFile file(plik);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw 1;

    QJsonObject root = QJsonObject();
    root["arx"] = arx.toJSON();

    QJsonDocument doc = QJsonDocument(root);
    file.write(doc.toJson());
    file.close();
}



void WarstaUslug::chooseAuthWithCode(int code)
{
    assert(isServerStarted());
    currentAuthCode = code;
    unsuccessfullAuthAttempts = 0;
    authenticated.value = false;
    sendDataPackage(AUTH_NEEDED);
    emit netLogAppend("Wysłano prośbę o wpisanie kodu do pratnera.");
}

void WarstaUslug::chooseAuthWithoutCode()
{
    assert(isServerStarted());
    authenticated.value = true;
    unsuccessfullAuthAttempts = 0;
    sendDataPackage(AUTH_SUCCESS);
    // po poprawnym polsczeniu wyslij konfiguracje
    sendFullConfig();
    emit connected();
    emit netLogAppend("Połączono w trybie bez autoryzacji.");
}
void  WarstaUslug::chooseAuthReject()
{
    assert(isServerStarted());
    disconnectGracefully();
    emit netLogAppend("Odrzucono połączenie z klientem.");
}
void WarstaUslug::authCodeVerification(int code)
{
    assert(isClientStarted());
    sendDataPackage(AUTH_CODE, QByteArray::number(code));
    emit netLogAppend("Przesłano kod do weryfikacji.");
}

void WarstaUslug::startAsServer(int port)
{
    if(server != nullptr)
    {
        server->close();
        server->deleteLater();
    }
    server = new QTcpServer(this);
    authenticated.value = false;
    unsuccessfullAuthAttempts = 0;
    this->port = port;
    QObject::connect(server, &QTcpServer::newConnection, this, &WarstaUslug::handleNewClient);
    if (server->listen(QHostAddress("0.0.0.0"), port))
    {
        emit netLogAppend("Serwer nasłuchuje na porcie " + QString::number(port));
        QObject::connect(&udpForSamples, &QUdpSocket::readyRead, this, [this](){
            processDataPackage(udpForSamples.receiveDatagram().data());
        });
        dziala.set(false);
    }
    else
        emit netLogAppend("ERROR: Port " + QString::number(port) + " wydaje się być zajęty!");
    emit updateUI();
}

void WarstaUslug::handleNewClient()
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
                    handleUnexpecteadDisconnection();
                });
        udpForSamples.bind(QHostAddress("0.0.0.0"), port);
        remoteIP.value = QHostAddress(socket->peerAddress().toIPv4Address()).toString();
        emit netLogAppend("Wykryto połączenie przychodzące z IP: " + remoteIP.get());
    }
    emit updateUI();
}

void WarstaUslug::connectAsClient(QString ip, int port)
{
    udpForSamples.bind(QHostAddress("0.0.0.0"), port);
    this->port = port;
    emit netLogAppend("Próba połączenia z " + ip + "...");
    QObject::connect(&udpForSamples, &QUdpSocket::readyRead, this, [this](){
        processDataPackage(udpForSamples.receiveDatagram().data());
    });

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, [this, port]()
            {
                sendDataPackage(CONNECTION_REQUEST);
                this->port = port;
            });
    connect(socket, &QTcpSocket::readyRead, this, [this]()
            {
                processDataPackage(socket->readAll());
            });
    connect(socket, &QTcpSocket::disconnected, this, [this]()
            {
                handleUnexpecteadDisconnection();
            });
    socket->connectToHost(ip, port);
    remoteIP.value = ip;
    emit updateUI();
}


void WarstaUslug::handleUnexpecteadDisconnection()
{
    unsuccessfullAuthAttempts = 0;
    authenticated.value = false;
    port = 0;
    udpForSamples.close();
    socket->close();
    if(!trybDzialania.hasOwnClock())
        dziala.set(true);
    trybDzialania.value = WarstaUslug::TrybDzialania::LOCAL;
    emit netLogAppend("Połączenie zerwane!");
    emit disconnected();
    emit updateUI();
}

void WarstaUslug::disconnectGracefully()
{
    if(isServerStarted() || isClientStarted())
        sendDataPackage(DISCONNECT_NOTIFY);
    socket->disconnectFromHost();
}
void WarstaUslug::sendText(QString message)
{
    assert(authenticated.get());
    sendDataPackage(TXT_MSG, message.toUtf8());
}

void WarstaUslug::sendSample(SimSampleFromRegulator sample)
{
    sendDataPackage(SIM_SAMPLE_FROM_REGULATOR, sample.toByteArray());
}
void WarstaUslug::sendSample(SimSampleFromObject sample)
{
    sendDataPackage(SIM_SAMPLE_FROM_OBJECT, sample.toByteArray());
}


void WarstaUslug::searchDevices()
{
    udpDiscovery.writeDatagram("UAR_QUERY", QHostAddress::Broadcast, CONSTS::NET::DISCOVERY_PORT);
    emit netLogAppend("Wysłano zapytanie DISCOVERY...");
}

void WarstaUslug::processDiscoveryUdp()
{
    while (udpDiscovery.hasPendingDatagrams())
    {
        QNetworkDatagram dg = udpDiscovery.receiveDatagram();
        QHostAddress sender = dg.senderAddress();

        sender = QHostAddress(sender.toIPv4Address());

        qDebug() << "DISCOVERY" << dg.data();
        if (dg.data() == "UAR_QUERY")
        {
            udpDiscovery.writeDatagram("UAR_RESP", QHostAddress::Broadcast, CONSTS::NET::DISCOVERY_PORT);
            emit netDeviceFound(sender.toString());
        }
        if (dg.data() == "UAR_RESP")
        {
            emit netDeviceFound(sender.toString());
        }
    }
}

void WarstaUslug::processDataPackage(QByteArray data)
{
    receivedPackets.value++;
    QDataStream in(&data, QIODevice::ReadOnly);
    quint8 type;
    in >> type;
    QByteArray dane_pakietu;
    in >> dane_pakietu;
    switch (type)
    {
    case CONNECTION_REQUEST:
        emit authChoiceQuestion();
        break;

    case AUTH_NEEDED:
        emit authCodeEntryRequired();
        break;

    case AUTH_FAILED:
        emit authErrorReceived();
        emit netLogAppend("Odrzucono próbę połączenia. Prawdopodobnie błędny kod.");
        break;

    case AUTH_CODE:
        if(dane_pakietu.toInt() == currentAuthCode)
        {
            sendDataPackage(AUTH_SUCCESS);
            authenticated.value = true;
            unsuccessfullAuthAttempts = 0;
            // po poprawnym polsczeniu wyslij konfiguracje
            sendFullConfig();
            emit connected();
            emit netLogAppend("Połączono klienta w trybie autoryzacji kodem.");
        }
        else
        {
            sendDataPackage(AUTH_FAILED);
            authenticated.value = false;
            unsuccessfullAuthAttempts++;
            emit netLogAppend("Nieudana próba połączenia z IP" + remoteIP.get());
        }
        if(unsuccessfullAuthAttempts >= CONSTS::NET::MAX_AUTH_ATTEMPTS)
        {
            emit netLogAppend("Otzymano 3 nieudane próby połączenia z IP " + remoteIP.get() + " zamykam połączenie i czekam na nowego klienta.");
            chooseAuthReject();
        }
        break;

    case AUTH_SUCCESS:
        unsuccessfullAuthAttempts = 0;
        authenticated.value = true;
        reset(false);
        emit netLogAppend("Połączenie udane! Tryb sieciowy aktywny.");
        emit connected();
        break;

    case TXT_MSG:
        emit netLogAppend("Otrzymano: " + QString::fromUtf8(dane_pakietu));
        break;

    case DISCONNECT_NOTIFY:
        emit netLogAppend("Partner zakończył połączenie.");
        break;

    case CONFIG_PID:
        disconnectIfNotAuthenticated();
        pid.fromByteArray(dane_pakietu);
        emit updateUI();
        break;

    case CONFIG_ARX:
        disconnectIfNotAuthenticated();
        arx.fromByteArray(dane_pakietu);
        emit updateUI();
        break;

    case CONFIG_GEN:
        disconnectIfNotAuthenticated();
        generator.fromByteArray(dane_pakietu);
        emit updateUI();
        break;

    case CONFIG_ONOFF:
        disconnectIfNotAuthenticated();
        onOff.fromByteArray(dane_pakietu);
        emit updateUI();
        break;

    case CONFIG_REGULATION:
        disconnectIfNotAuthenticated();
        regulacja.fromByteArray(dane_pakietu);
        emit updateUI();
        break;

    case CONFIG_INTERVAL:
        disconnectIfNotAuthenticated();
        interwal.fromByteArray(dane_pakietu);
        emit updateUI();
        break;

    case SIM_SAMPLE_FROM_OBJECT:
        disconnectIfNotAuthenticated();
        {
            SimSampleFromObject sample = SimSampleFromObject::fromByteArray(dane_pakietu);
            if(sample.czas != this->czas)
            {
                return;
            }
            oczekujacy_tick.extrapolated = false;
            oczekujacy_tick.wartoscRegulowana = sample.wartoscRegulowana;
            uar.zaktualizujPoprzendieWyjscie(sample.wartoscRegulowana);
            ostatnia_rzeczywista_wartosc_z_obiektu_arx = sample.wartoscRegulowana;
            emit symulacjaWyrabiaSie(true);
            emit updateCharts(oczekujacy_tick, czas);
        }
        break;

    case SIM_SAMPLE_FROM_REGULATOR:
        disconnectIfNotAuthenticated();
        {
            SimSampleFromRegulator sample = SimSampleFromRegulator::fromByteArray(dane_pakietu);
            czas = sample.czas;

            double wartoscRegulowana = uar.symulujObiekt(sample.sterowanie, sample.wartoscZadana, interwal.get());
            UAR::Tick tick;
            tick.wartoscRegulowana = wartoscRegulowana;
            tick.wartoscZadana = sample.wartoscZadana;
            tick.sterowanie = sample.sterowanie;
            tick.uchyb = 0.0; // TYMCZASOWO
            tick.pid = std::nullopt;
            tick.extrapolated = false;
            sendSample(SimSampleFromObject {wartoscRegulowana, sample.czas});
            emit updateCharts(tick, sample.czas);
        }
        break;

    case SIM_RUNNING:
        disconnectIfNotAuthenticated();
        if(trybDzialania.hasOwnClock())
            dziala.set(static_cast<bool>(dane_pakietu.toInt()));
        break;
    case SIM_RESTART:
        disconnectIfNotAuthenticated();
        reset(false);
        emit simmulationRestarted();
        emit updateUI();
        break;
    }
}

void  WarstaUslug::sendDataPackage(quint8 type, const QByteArray &data)
{
    assert(socket != nullptr); // Jest połączenie

    QByteArray package;
    QDataStream out(&package, QIODevice::WriteOnly);
    out << type << data;
    tansmitedPackets.value++;

    if(false)
    // if(type == SIM_SAMPLE_FROM_OBJECT || type == SIM_SAMPLE_FROM_REGULATOR)
    {
        udpForSamples.writeDatagram(package, QHostAddress(remoteIP.get()), this->port);
    }
    else
    {
        if(socket->isOpen())
        {
            socket->write(package);
            socket->flush();
        }
        else
            emit netLogAppend("ERROR: Połączenie nie jest otwarte, a próbowano wysłać pakiet");
    }

}
void WarstaUslug::disconnectIfNotAuthenticated()
{
    if(!authenticated.get())
    {
        disconnectGracefully();
    }
}

bool WarstaUslug::isServerStarted()
{
    return server != nullptr;
}
bool WarstaUslug::isClientStarted()
{
    return (socket != nullptr && server == nullptr);
}

void WarstaUslug::sendFullConfig()
{
    sendDataPackage(CONFIG_INTERVAL, interwal.toByteArray());
    sendDataPackage(CONFIG_REGULATION, regulacja.toByteArray());
    sendDataPackage(CONFIG_ARX, arx.toByteArray());
    sendDataPackage(CONFIG_PID, pid.toByteArray());
    sendDataPackage(CONFIG_ONOFF, onOff.toByteArray());
    sendDataPackage(CONFIG_GEN, generator.toByteArray());
}

