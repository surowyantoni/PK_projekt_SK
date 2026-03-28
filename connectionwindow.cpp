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
    connect(service, &NetService::deviceFound, this, [this](QString ip){
        if (ui->comboIP->findText(ip) == -1) ui->comboIP->addItem(ip);
    });
    connect(service, &NetService::authRequired, this, &ConnectionWindow::onAuthRequired);

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
    ui->comboIP->clear();
    service->searchDevices();
}

//Ustawienie trybu. Jeżeli ustawimy na serwer to go uruchamiamy, jeżeli klient - zamykamy serwer i ustawiamy klienta
void ConnectionWindow::on_radioServer_toggled(bool checked)
{
    service->stopAll();
    if (checked)
        service->startAsServer(ui->spinPort->value());
    else
        service->startAsClient(localIP, ui->spinPort->value());
}

void ConnectionWindow::on_btnConnect_clicked()
{
    QString ip = ui->comboIP->currentText();
    if (ip.isEmpty())
        ip = composeIPAddres();

    service->startAsClient(ip, ui->spinPort->value());
}

void ConnectionWindow::on_btnDisconnect_clicked()
{
    int res = QMessageBox::question(this, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?");

    if (res == QMessageBox::Yes)
    {
        log("Zażądano rozłączenia z partnerem.");
        service->stopAll(); // Zlecenie zatrzymania usług sieciowych [Hist.]
        updateStatus(false); // Aktualizacja GUI [4]
    }
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

void ConnectionWindow::onAuthRequired(QString ip)
{
    QMessageBox msgBox;
    msgBox.setText("Zapytanie od: " + ip);
    msgBox.setInformativeText("Czy przejść w tryb sieciowy?");
    msgBox.addButton("Generuj kod", QMessageBox::AcceptRole);
    msgBox.addButton("Odrzuć", QMessageBox::RejectRole);
    int result = msgBox.exec();

    if (result == QMessageBox::AcceptRole)
    {
        int code = QRandomGenerator::global()->bounded(1000, 9999);
        service->verifyCode(QString::number(code));
        log("Wygenerowano kod: " + QString::number(code));
    } else service->stopAll();
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}