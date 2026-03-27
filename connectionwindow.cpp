#include "connectionwindow.h"
#include "ui_connectionwindow.h"

#include <QRandomGenerator>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTime>

ConnectionWindow::ConnectionWindow(NetService *net, QWidget *parent)
    : QDialog(parent), ui(new Ui::ConnectionWindow)
    , service(net)
{
    ui->setupUi(this);
    setWindowTitle("Połączenie sieciowe");

    connect(service, &NetService::logAppend, this, &ConnectionWindow::log);
    connect(service, &NetService::updateStatus, this, &ConnectionWindow::updateStatus);

    for (const QHostAddress &address : QNetworkInterface::allAddresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            localIP = address.toString();

    ui->labelLocalIP->setText("IP lokalne: " + localIP);
    log("Uruchomiono okno połączeń...");
}

// MODE_CHECK - sprawdzamy stan po drugiej stronie
// MODE_NETWORK - odpowiedz stan sieciowy
// MODE_STATIONARY - odpowiedz stan stacjonarny
// MODE_SWITCH_REQUEST - prosba o zmiane stanu
// MODE_SWITCH_ACCEPT - akceptacja zmiany
// MODE_SWITCH_DENY - odrzucenie zmiany
// CONNECT_NO_CODE - polaczenie bez kodu
// CONNECT_WITH_CODE - polaczenie z kodem
// CODE_REQUEST - prosba o kod
// CODE_OK - kod poprawny
// CODE_BAD - kod niepoprawny
// DISCONNECT - rozlaczenie

//Wypisujemy log w boxie z informacją co się dzieje (stan np próba łączenia itp.)
void ConnectionWindow::log(QString text)
{
    ui->logBox->append("[" + QTime::currentTime().toString("HH:mm:ss") + "] " + text);
}

QString ConnectionWindow::composeIPAddres()
{
    QString address = ui->editIP->text() + "." + ui->editIP2->text() + "." + ui->editIP3->text()+ "." + ui->editIP4->text();
    return address;
}

void ConnectionWindow::decomposeIPAddres()
{
    QString address = ui->comboIP->currentText();

    QStringList parts = address.split(".");

    if(parts.size() == 4)
    {
        ui->editIP->setText(parts[0]);
        ui->editIP2->setText(parts[1]);
        ui->editIP3->setText(parts[2]);
        ui->editIP4->setText(parts[3]);
    }
    else
    {
        ui->editIP->clear();
        ui->editIP2->clear();
        ui->editIP3->clear();
        ui->editIP4->clear();
    }
}

void ConnectionWindow::on_btnSearch_clicked()
{
    log("Skanowanie sieci...");
    decomposeIPAddres();
}

//Ustawienie trybu. Jeżeli ustawimy na serwer to go uruchamiamy, jeżeli klient - zamykamy serwer i ustawiamy klienta
void ConnectionWindow::on_radioServer_toggled(bool checked)
{
    if (checked)
        service->startAsServer(ui->spinPort->value());
    else
        service->startAsClient(localIP, ui->spinPort->value());
}

void ConnectionWindow::on_btnConnect_clicked()
{

}

void ConnectionWindow::on_btnDisconnect_clicked()
{

}

void ConnectionWindow::updateStatus(bool connected)
{
    if(connected)
    {
        ui->labelConnStatus->setText("● Połączono");
        ui->labelConnStatus->setStyleSheet("color: green;");
        //ui->labelRemoteIP->setText("IP zdalne: " + socket->peerAddress().toString());
    }
    else
    {
        ui->labelConnStatus->setText("● Rozłączono");
        ui->labelConnStatus->setStyleSheet("color: red;");
        ui->labelRemoteIP->setText("IP zdalne: -");
    }
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}