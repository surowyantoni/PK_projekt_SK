#include "connectionwindow.h"
#include "ProtocolDef.h"
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
    connect(service, &NetService::deviceFound, this, &ConnectionWindow::onDeviceFound);
    connect(service, &NetService::authRequired, this, &ConnectionWindow::onAuthRequired);
    connect(service, &NetService::authErrorReceived, this, &ConnectionWindow::onAuthError);
    connect(service, &NetService::codeEntryRequired, this, &ConnectionWindow::onCodeEntryRequired);

    connect(ui->editIP, &QLineEdit::editingFinished, this, &ConnectionWindow::setComboIPnewAddress);
    connect(ui->editIP2, &QLineEdit::editingFinished, this, &ConnectionWindow::setComboIPnewAddress);
    connect(ui->editIP3, &QLineEdit::editingFinished, this, &ConnectionWindow::setComboIPnewAddress);
    connect(ui->editIP4, &QLineEdit::editingFinished, this, &ConnectionWindow::setComboIPnewAddress);
    connect(ui->editIP, &QLineEdit::textChanged, this, [this](QString t){ if (t.length() == 3) ui->editIP2->setFocus(); });
    connect(ui->editIP2, &QLineEdit::textChanged, this, [this](QString t){ if (t.length() == 3) ui->editIP3->setFocus(); });
    connect(ui->editIP3, &QLineEdit::textChanged, this, [this](QString t){ if (t.length() == 3) ui->editIP4->setFocus(); });
    connect(ui->editIP4, &QLineEdit::returnPressed, this, &ConnectionWindow::setComboIPnewAddress);

    for (const QHostAddress &address : QNetworkInterface::allAddresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            localIP = address.toString();

    ui->labelLocalIP->setText("IP lokalne: " + localIP);
    log("Uruchomiono okno połączeń...");
}

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

bool ConnectionWindow::isIPValid(QString ip)
{
    QStringList parts = ip.split(".");
    if (parts.size() != 4) return false;

    for (const QString &p : parts)
    {
        bool ok;
        int num = p.toInt(&ok);
        if (!ok || num < 0 || num > 255)
            return false;
    }
    return true;
}

void ConnectionWindow::setComboIPnewAddress()
{
    QString ip = composeIPAddres();

    if (!isIPValid(ip))
        return;

    int index = ui->comboIP->currentIndex();

    if (index >= 0)
        ui->comboIP->setItemText(index, ip);
    else
    {
        if (ui->comboIP->findText(ip) == -1)
        {
            ui->comboIP->addItem(ip);
            index = ui->comboIP->count() - 1;
        }
        else index = ui->comboIP->findText(ip);
    }

    ui->comboIP->setCurrentIndex(index);
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
    service->searchDevices();
}

void ConnectionWindow::onDeviceFound(QString ip)
{
    if (ui->comboIP->findText(ip) == -1 && ip != localIP)
    {
        ui->comboIP->addItem(ip);
        log("Znaleziono urządzenie: " + ip);
    } else log("Brak nowych urządzeń.");
}

//Ustawienie trybu. Jeżeli ustawimy na serwer to go uruchamiamy, jeżeli klient - zamykamy serwer i ustawiamy klienta
void ConnectionWindow::on_radioServer_toggled(bool checked)
{
    service->stopAll();
    if (checked)
    {
        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(true);
        ui->btnConnect->setEnabled(false);
        ui->btnDisconnect->setEnabled(false);
    }
    else
    {
        ui->btnStart->setEnabled(false);
        ui->btnStop->setEnabled(false);
        ui->btnConnect->setEnabled(true);
        ui->btnDisconnect->setEnabled(true);
    }

}

void ConnectionWindow::on_btnConnect_clicked()
{
    QString ip = ui->comboIP->currentText();
    if (ip.isEmpty())
        ip = composeIPAddres();

    if (ui->comboMode->currentText() != "STACJONARNY")
    {
        log("Próba połączenia z " + ip + "...");
        service->startAsClient(ip, ui->spinPort->value());
        ui->btnSend->setEnabled(true);
        ui->comboMode->setCurrentIndex(1);
    }

}

void ConnectionWindow::on_btnDisconnect_clicked()
{
    int res = QMessageBox::question(this, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?");

    if (res == QMessageBox::Yes)
    {
        log("Zażądano rozłączenia z partnerem.");
        service->stopAll();                         // Zlecenie zatrzymania usług sieciowych
        updateStatus(false);
        ui->btnSend->setEnabled(false);
    }

    ui->comboMode->setCurrentIndex(0);
}

void ConnectionWindow::on_btnStart_clicked()
{
    if (ui->comboMode->currentText() != "STACJONARNY")
    {
        service->startAsServer(ui->spinPort->value());
        ui->comboMode->setCurrentIndex(1);
        ui->btnSend->setEnabled(true);
    }
}

void ConnectionWindow::on_btnStop_clicked()
{
    int res = QMessageBox::question(this, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?");

    if (res == QMessageBox::Yes)
    {
        log("Wyłączanie serwera...");
        service->stopAll();                         // Zlecenie zatrzymania usług sieciowych
        updateStatus(false);
        ui->btnSend->setEnabled(false);
    }

    ui->comboMode->setCurrentIndex(0);
}


void ConnectionWindow::on_comboMode_currentIndexChanged(int index)
{

    if (ui->comboMode->currentText() != "STACJONARNY")
    {
        int res = QMessageBox::question(this, "Zmiana trybu!", "Czy na pewno chcesz przejść w tryb sieciowy?");

        if (res == QMessageBox::Yes)
        {
            //TO DO - zablokowanie kontrolek
            ui->comboMode->setCurrentIndex(1);
        }
        else ui->comboMode->setCurrentIndex(0);
    }
}

void ConnectionWindow::on_comboIP_currentTextChanged(const QString &arg1)
{
    decomposeIPAddres();
}

void ConnectionWindow::on_btnClear_clicked()
{
    ui->logBox->clear();
}

void ConnectionWindow::on_btnSend_clicked()
{
    service->sendPacket(TXT_MSG, ui->editMsg->text());
    log("Wysłano: " + ui->editMsg->text());
    ui->editMsg->clear(); ui->editMsg->setFocus();
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
    msgBox.setText("Urządzenie " + ip + " chce się połączyć.");
    msgBox.setInformativeText("Wybierz tryb autoryzacji:");
    QPushButton *btnCode = msgBox.addButton("Generuj kod", QMessageBox::ActionRole);
    QPushButton *btnNoCode = msgBox.addButton("Bez kodu", QMessageBox::ActionRole);
    QPushButton *deny = msgBox.addButton("Odrzuć", QMessageBox::RejectRole);
    msgBox.exec();

    if (msgBox.clickedButton() == btnCode)
    {
        QString code = QString::number(QRandomGenerator::global()->bounded(1000, 9999));
        log("Wygenerowano kod dla partnera: " + code);
        service->setAuthMode(0, code); // Tryb z kodem
    }
    else if (msgBox.clickedButton() == btnNoCode) { service->setAuthMode(1, ""); }
    else { service->sendPacket(CODE_DENY); }
}

void ConnectionWindow::onAuthError(int attempt)
{
    QMessageBox::warning(this, "Błąd autoryzacji", QString("Podano błędny kod! Próba %1 z 3.").arg(attempt));
    onCodeEntryRequired();
}

void ConnectionWindow::onCodeEntryRequired()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Autoryzacja", "Partner wymaga kodu dostępu:", QLineEdit::Normal, "", &ok);

    if (ok && !text.isEmpty())
    {
        log("Wysłano kod do weryfikacji...");
        service->sendCodeToCheck(text);
    } else { service->sendPacket(CODE_DENY); }
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}
