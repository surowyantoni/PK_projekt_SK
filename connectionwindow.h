#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include "netservice.h"
#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

namespace Ui { class ConnectionWindow; }

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionWindow(NetService *net, QWidget *parent = nullptr);
    ~ConnectionWindow();

private slots:
    void on_btnConnect_clicked();                   //Połącz -> serwis
    void on_btnDisconnect_clicked();                //Rozłącz -> serwis
    void on_btnSearch_clicked();                    //Szukaj urządzeń -> serwis
    void on_radioServer_toggled(bool checked);      //Server start/stop -> serwis

    void log(QString text);                         //Logowanie
    void updateStatus(bool connected);              //UI status
    void onAuthRequired(QString ip);                //Wyrzucenie okna
    void onDeviceFound(QStringList ip_list);                 //Wpisanie znalezionychdo comboBox
private:
    Ui::ConnectionWindow *ui;
    QString localIP;                                //Lokalne IP
    NetService *service = nullptr;                  //Serwis

    QString composeIPAddres();                      //Złączenie IP w 1 ciąg
    void decomposeIPAddres();                       //Rozbicie IP na 4 pola (".")
};

#endif // CONNECTIONWINDOW_H