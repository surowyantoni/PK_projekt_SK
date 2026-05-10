#include "connectionwindow.h"
#include "qevent.h"
#include "ui_connectionwindow.h"

#include <QRandomGenerator>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTime>

ConnectionWindow::ConnectionWindow(WarstaUslug& uslugi_i, QWidget *parent)
    : QDialog(parent), ui(new Ui::ConnectionWindow)
    , uslugi{uslugi_i}
{
    ui->setupUi(this);
    setWindowTitle("Połączenie sieciowe");
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

    connect(&uslugi.netService, &NetService::logAppend, this, &ConnectionWindow::log);
    connect(&uslugi.netService, &NetService::connectionStatusChanged, this, &ConnectionWindow::updateStatus);
    connect(&uslugi.netService, &NetService::deviceFound, this, &ConnectionWindow::onDeviceFound);
    connect(&uslugi.netService, &NetService::authQuestionForUser, this, &ConnectionWindow::onAuthRequired);
    connect(&uslugi.netService, &NetService::authErrorReceived, this, &ConnectionWindow::onAuthError);
    connect(&uslugi.netService, &NetService::authCodeEntryRequired, this, &ConnectionWindow::onCodeEntryRequired);
    connect(&uslugi, &WarstaUslug::updateUI, this, &ConnectionWindow::updateUI);

    connect(ui->spinBox_ip1,  &QSpinBox::textChanged, this, [this](QString t){ if (t.length() == 3) ui->spinBox_ip2->setFocus(); });
    connect(ui->spinBox_ip2, &QSpinBox::textChanged, this, [this](QString t){ if (t.length() == 3) ui->spinBox_ip3->setFocus(); });
    connect(ui->spinBox_ip3, &QSpinBox::textChanged, this, [this](QString t){ if (t.length() == 3) ui->spinBox_ip4->setFocus(); });

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
    return ui->spinBox_ip1->text() + "." + ui->spinBox_ip2->text() + "." + ui->spinBox_ip3->text()+ "." + ui->spinBox_ip4->text();
}

void ConnectionWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if (this->windowState() & Qt::WindowMinimized)
        {
            this->hide();
            //Tutaj możemy zrobić customową logikę co i gdzie ma się pojawić po minimalizacj
            //aktualnie otwierasz okno z menu
        }
    }

    QDialog::changeEvent(event);
}

void ConnectionWindow::closeEvent(QCloseEvent *event)
{
    int res = QMessageBox::question(this, "Zamykanie...", "Czy na pewno chcesz zamknąć okno? Zerwie to aktywne połączenia!");

    if (res == QMessageBox::Yes)
    {
        uslugi.netService.disconnect();
        event->accept();
    } else event->ignore();
}

void ConnectionWindow::decomposeIPAddres(QString ip)
{

    QStringList parts = ip.split(".");

    if(parts.size() == 4)
    {
        ui->spinBox_ip1->setValue(parts[0].toInt());
        ui->spinBox_ip2->setValue(parts[1].toInt());
        ui->spinBox_ip3->setValue(parts[2].toInt());
        ui->spinBox_ip4->setValue(parts[3].toInt());
    }
    else
    {
        ui->spinBox_ip1->clear();
        ui->spinBox_ip2->clear();
        ui->spinBox_ip3->clear();
        ui->spinBox_ip4->clear();
    }
}

void ConnectionWindow::onDeviceFound(QString ip)
{
    if (ui->combo_znalezione->findText(ip) == -1 && ip != localIP)
    {
        ui->combo_znalezione->addItem(ip);
        log("Znaleziono urządzenie: " + ip);
    } else log("Brak nowych urządzeń.");
}


void ConnectionWindow::on_pushButton_clear_clicked()
{
    ui->logBox->clear();
}

void ConnectionWindow::on_pushButton_Send_clicked()
{
    uslugi.netService.sendText(ui->lineEdit_wiadomosc->text());
    log("Wysłano: " + ui->lineEdit_wiadomosc->text());
    ui->lineEdit_wiadomosc->clear();
    ui->lineEdit_wiadomosc->setFocus();
}

void ConnectionWindow::updateStatus(bool connected, QString ip)
{
    ui->pushButton_Send->setEnabled(connected);
    ui->pushButton_connection->setEnabled(connected);
    if(connected)
    {
        ui->pushButton_connection->setText("Rozłącz");
        ui->labelConnStatus->setText("● Połączono");
        ui->labelConnStatus->setStyleSheet("color: green;");
        ui->labelRemoteIP->setText("IP zdalne: " + ip);
    }
    else
    {
        ui->pushButton_connection->setText("Połącz");
        ui->labelConnStatus->setText("● Rozłączono");
        ui->labelConnStatus->setStyleSheet("color: red;");
        ui->labelRemoteIP->setText("IP zdalne: -");
    }
}
void ConnectionWindow::updateUI()
{
    bool tryb_sieciowy = uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL;
    ui->pushButton_connection->setEnabled(tryb_sieciowy);
    ui->lineEdit_wiadomosc->setEnabled(tryb_sieciowy);
    ui->pushButton_Send->setEnabled(tryb_sieciowy);
    ui->pushButton_search->setEnabled(tryb_sieciowy);
    ui->spinBox_ip1->setEnabled(tryb_sieciowy);
    ui->spinBox_ip2->setEnabled(tryb_sieciowy);
    ui->spinBox_ip3->setEnabled(tryb_sieciowy);
    ui->spinBox_ip4->setEnabled(tryb_sieciowy);
    ui->spinBox_port->setEnabled(tryb_sieciowy);
    ui->combo_znalezione->setEnabled(tryb_sieciowy);
}

void ConnectionWindow::onAuthRequired()
{
    QMessageBox msgBox;
    msgBox.setText("Urządzenie  chce się połączyć.");
    msgBox.setInformativeText("Wybierz tryb autoryzacji:");
    QPushButton *btnCode = msgBox.addButton("Generuj kod", QMessageBox::ActionRole);
    QPushButton *btnNoCode = msgBox.addButton("Bez kodu", QMessageBox::ActionRole);
    QPushButton *deny = msgBox.addButton("Odrzuć", QMessageBox::RejectRole);
    msgBox.exec();

    if (msgBox.clickedButton() == btnCode)
    {
        int code = QRandomGenerator::global()->bounded(1000, 9999);
        log("Wygenerowano kod dla partnera: " + QString::number(code));
        uslugi.netService.chooseAuthWithCode(code); // Tryb z kodem
    }
    else if (msgBox.clickedButton() == btnNoCode) { uslugi.netService.chooseAuthWithoutCode(); }
    else { uslugi.netService.chooseAuthReject(); }
}

void ConnectionWindow::onAuthError(QString errMsg)
{
    QMessageBox::warning(this, "Błąd autoryzacji", QString("Podano błędny kod! %1.").arg(errMsg));
    onCodeEntryRequired();
}

void ConnectionWindow::onCodeEntryRequired()
{
    bool ok;
    QString text;
    better_luck_next_time:
    text = QInputDialog::getText(this, "Autoryzacja", "Partner wymaga kodu dostępu:", QLineEdit::Normal, "", &ok);

    if (ok && !text.isEmpty())
    {
        uslugi.netService.authCodeVerification(text.toInt());
        log("Wysłano kod do weryfikacji...");
    }
    else
    {
        log("Wprowadzono niepoprawny format kodu");
        goto better_luck_next_time;
    }
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}

void ConnectionWindow::on_radioLokalny_clicked()
{
    if(uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::LOCAL)
        return;

    if(!uslugi.netService.isAuthenticated())
    {
        uslugi.netService.startLocal();
        return;
    }

    if (QMessageBox::question(this, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?") == QMessageBox::Yes)
    {
        log("Zażądano rozłączenia z partnerem.");
        uslugi.netService.disconnect();                         // Zlecenie zatrzymania usług sieciowych
    }
    else
    {
        QSignalBlocker radio1(ui->radioClient);
        QSignalBlocker radio2(ui->radioServer);
        if(uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_ARX)
            ui->radioClient->setChecked(true);
        else
            ui->radioServer->setChecked(true);
    }

    uslugi.trybDzialania.set(WarstaUslug::TrybDzialania::LOCAL);
    ui->pushButton_connection->setEnabled(false);
}


void ConnectionWindow::on_radioServer_clicked()
{
    if(uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_REG)
        return;
    if(uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_ARX && uslugi.netService.isAuthenticated())
    {
        if (QMessageBox::question(this, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?") == QMessageBox::Yes)
        {
            log("Zażądano rozłączenia z partnerem.");
            uslugi.netService.disconnect();
        }
        else
        {
            QSignalBlocker radio1(ui->radioClient);
            QSignalBlocker radio2(ui->radioServer);
            ui->radioClient->setChecked(true);
            return;
        }
    }
    uslugi.trybDzialania.set(WarstaUslug::TrybDzialania::NET_REG);
    ui->pushButton_connection->setEnabled(true);
}


void ConnectionWindow::on_radioClient_clicked()
{
    if(uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_ARX)
        return;
    if(uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_REG && uslugi.netService.isAuthenticated())
    {
        if (QMessageBox::question(this, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?") == QMessageBox::Yes)
        {
            log("Zażądano rozłączenia z partnerem.");
            uslugi.netService.disconnect();
        }
        else
        {
            QSignalBlocker radio1(ui->radioClient);
            QSignalBlocker radio2(ui->radioServer);
            ui->radioClient->setChecked(true);
            return;
        }
    }
    uslugi.trybDzialania.set(WarstaUslug::TrybDzialania::NET_ARX);
    ui->pushButton_connection->setEnabled(true);
}

void ConnectionWindow::setBufferFill(int percentage)
{
    ui->progressBarOpoznienie->setValue(percentage);
}


void ConnectionWindow::on_pushButton_search_clicked()
{
    log("Skanowanie sieci...");
    uslugi.netService.searchDevices();
}


void ConnectionWindow::on_pushButton_connection_clicked()
{
    if(uslugi.netService.isAuthenticated())
    {
        int res = QMessageBox::question(this, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?");

        if (res == QMessageBox::Yes)
        {
            log("Wyłączanie serwera...");
            uslugi.netService.disconnect();                         // Zlecenie zatrzymania usług sieciowych
        }
        return;
    }
    else
    {
        if(uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_ARX)
        {
            QString ip = composeIPAddres();
            log("Próba połączenia z " + ip + "...");
            uslugi.netService.connectAsClient(ip, ui->spinBox_port->value());
        }
        else if(uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_REG)
        {
            log("Próba uruchomienia serwera...");
            uslugi.netService.startAsServer(ui->spinBox_port->value());
        }
    }
    ui->pushButton_connection->setEnabled(false);
}


void ConnectionWindow::on_combo_znalezione_currentTextChanged(const QString &arg1)
{
    decomposeIPAddres(arg1);
}

