#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void listen(int port);
    void stopListening();
    void sendTo(int id, QByteArray data);

signals:
    void newConn(int id, QString ip);
    void dataReceived(int id, QByteArray data);

private slots:
    void onNewConnection();

private:
    QTcpServer server;
    QVector<QTcpSocket*> clients;
};

#endif // SERVER_H
