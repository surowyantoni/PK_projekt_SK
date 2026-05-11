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
    explicit ConnectionWindow(WarstaUslug& uslugi, QWidget *parent = nullptr);
    ~ConnectionWindow();

protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:    
    void updateUI(); // Aktywowane przez warstwę usług, ustawia aktywność kontorlek na skutek zmiany ustawień trybu pracy
    void log(QString text);
    void onConnected();  //UI status
    void onDisconnected();  //UI status
    void onAuthChoiceRequired();//Wyrzucenie okna z pytaniem o sposób połączenia
    void onAuthError();//Reakcja na zły kod
    void onCodeEntry();      //Prośba o podanie kodu
    void onDeviceFound(QString ip);  //Wpisanie znalezionychdo comboBox
    void onNetwokrError(QString errorMessage);

    void on_radioLokalny_clicked();

    void on_radioServer_clicked();

    void on_radioClient_clicked();

    void on_pushButton_search_clicked();

    void on_pushButton_connection_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_Send_clicked();

    void on_combo_znalezione_currentTextChanged(const QString &arg1);

private:
    Ui::ConnectionWindow *ui;
    WarstaUslug& uslugi;
    QTimer statsTimer;

    void setBufferFill(int percentage);
    QString composeIPAddres();  //Złączenie IP w 1 ciąg
    void decomposeIPAddres(QString ip);   //Rozbicie IP na 4 pola (".")
};

#endif // CONNECTIONWINDOW_H
