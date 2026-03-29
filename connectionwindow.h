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
    void onAuthError(int attempt);                  //Reakcja na zły kod
    void onCodeEntryRequired();                     //Prośba o podanie kodu
    void onDeviceFound(QString ip);                 //Wpisanie znalezionychdo comboBox
    void on_comboIP_currentTextChanged(const QString &arg1);

    void on_btnClear_clicked();

    void on_btnStart_clicked();

    void on_btnStop_clicked();

    void on_comboMode_currentIndexChanged(int index);

    void on_btnSend_clicked();

private:
    Ui::ConnectionWindow *ui;
    QString localIP;                                //Lokalne IP
    NetService *service = nullptr;                  //Serwis

    QString composeIPAddres();                      //Złączenie IP w 1 ciąg
    void decomposeIPAddres();                       //Rozbicie IP na 4 pola (".")
    void setComboIPnewAddress();                    //Ustawia nowy element z wpisanego IP do comboBox
    bool isIPValid(QString ip);
};

#endif // CONNECTIONWINDOW_H