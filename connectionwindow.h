#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include "WarstwaUslug.h"
#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

namespace Ui { class ConnectionWindow; }

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionWindow(WarstaUslug *uslugi, QWidget *parent = nullptr);
    ~ConnectionWindow();
    void setBufferFill(int percentage);
protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_btnSearch_clicked();

    void log(QString text);                         //Logowanie
    void updateStatus(bool connected, QString ip);  //UI status
    void onAuthRequired();                          //Wyrzucenie okna z pytaniem o sposób połączenia
    void onAuthError(QString errMsg);               //Reakcja na zły kod
    void onCodeEntryRequired();                     //Prośba o podanie kodu
    void onDeviceFound(QString ip);                 //Wpisanie znalezionychdo comboBox
    void on_comboIP_currentTextChanged(const QString &arg1);

    void on_btnClear_clicked();

    void on_btnStart_clicked();

    void on_btnStop_clicked();

    void on_btnSend_clicked();

    void on_radioLokalny_clicked();

    void on_radioServer_clicked();

    void on_radioClient_clicked();


private:
    Ui::ConnectionWindow *ui;
    QString localIP;
    WarstaUslug *uslugi = nullptr;

    QString composeIPAddres();  //Złączenie IP w 1 ciąg
    void decomposeIPAddres();   //Rozbicie IP na 4 pola (".")
    void setComboIPnewAddress();//Ustawia nowy element z wpisanego IP do comboBox
    bool isIPValid(QString ip);
};

#endif // CONNECTIONWINDOW_H
