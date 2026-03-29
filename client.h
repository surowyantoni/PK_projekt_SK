#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void connectTo(QString address, int port);          //Podłącz do
    void disconnect();                                  //Rozłącz
    void sendData(QByteArray data);
    bool isConnected() { return socket.isOpen(); }    //Sprawdzenie poprawności połączenia

signals:
    void connected();                                   //
    void dataRecieved(QByteArray data);                 //
    void disconnected();                                //

private:
    QTcpSocket socket;                                  //Socket klienta
};

#endif // CLIENT_H
