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
    , statsTimer{QTimer()}
{
    ui->setupUi(this);
    statsTimer.setInterval(CONSTS::NET::STATS_UPDATE_INTERVAL);
    QObject::connect(&statsTimer, &QTimer::timeout, this, [this](){
        ui->labelPacketsReceived->setText("RX: " + QString::number(uslugi.receivedPackets.get()));
        ui->labelPacketsSent->setText("TX: " + QString::number(uslugi.tansmitedPackets.get()));
    });


    setWindowTitle("Połączenie sieciowe");
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

    connect(&uslugi, &WarstaUslug::netLogAppend, this, &ConnectionWindow::log);
    connect(&uslugi, &WarstaUslug::connected, this, &ConnectionWindow::onConnected);
    connect(&uslugi, &WarstaUslug::disconnected, this, &ConnectionWindow::onDisconnected);
    connect(&uslugi, &WarstaUslug::netDeviceFound, this, &ConnectionWindow::onDeviceFound);
    connect(&uslugi, &WarstaUslug::authChoiceQuestion, this, &ConnectionWindow::onAuthChoiceRequired);
    connect(&uslugi, &WarstaUslug::authErrorReceived, this, &ConnectionWindow::onAuthError);
    connect(&uslugi, &WarstaUslug::authCodeEntryRequired, this, &ConnectionWindow::onCodeEntry);
    connect(&uslugi, &WarstaUslug::netError, this, &ConnectionWindow::onNetwokrError);
    connect(&uslugi, &WarstaUslug::updateUI, this, &ConnectionWindow::updateUI);

    connect(ui->spinBox_ip1,  &QSpinBox::textChanged, this, [this](QString t){ if (t.length() == 3) ui->spinBox_ip2->setFocus(); });
    connect(ui->spinBox_ip2, &QSpinBox::textChanged, this, [this](QString t){ if (t.length() == 3) ui->spinBox_ip3->setFocus(); });
    connect(ui->spinBox_ip3, &QSpinBox::textChanged, this, [this](QString t){ if (t.length() == 3) ui->spinBox_ip4->setFocus(); });



    ui->labelLocalIP->setText("IP lokalne: " + uslugi.localIP.get());
    log("Uruchomiono okno połączeń...");
    onDisconnected();
    updateUI();
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

void ConnectionWindow::onNetwokrError(QString errorMessage)
{
    QMessageBox::critical(this, "Błąd sieci", errorMessage);
}

void ConnectionWindow::closeEvent(QCloseEvent *event)
{
    if(uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::LOCAL)
    {
        event->accept();
        return;
    }

    int res = QMessageBox::question(this, "Zamykanie...", "Czy na pewno chcesz zamknąć okno? Zerwie to aktywne połączenia!");

    if (res == QMessageBox::Yes)
    {
        uslugi.trybDzialania.set(WarstaUslug::TrybDzialania::LOCAL);
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
    if (ui->combo_znalezione->findText(ip) == -1)
    {
        ui->combo_znalezione->addItem(ip);
        log("Znaleziono urządzenie: " + ip);
    } else log("Brak nowych urządzeń.");
}


void ConnectionWindow::on_pushButton_clear_clicked()
{
    ui->logBox->clear();
    log("Uruchomiono okno połączeń...");
}

void ConnectionWindow::on_pushButton_Send_clicked()
{
    uslugi.sendText(ui->lineEdit_wiadomosc->text());
    log("Wysłano: " + ui->lineEdit_wiadomosc->text());
    ui->lineEdit_wiadomosc->clear();
    ui->lineEdit_wiadomosc->setFocus();
}

void ConnectionWindow::onConnected()
{
    ui->pushButton_Send->setEnabled(true);
    ui->labelConnStatus->setText("● Połączono");
    ui->labelConnStatus->setStyleSheet("color: green;");
    ui->labelRemoteIP->setText("IP zdalne: " + uslugi.remoteIP.get());
    statsTimer.start();
    emit uslugi.updateUI();

}

void ConnectionWindow::onDisconnected()
{
    ui->pushButton_Send->setEnabled(false);
    ui->labelConnStatus->setText("● Rozłączono");
    ui->labelConnStatus->setStyleSheet("color: red;");
    ui->labelRemoteIP->setText("IP zdalne: -");
    ui->labelPacketsReceived->setText("RX: -");
    ui->labelPacketsSent->setText("TX: -");
    statsTimer.stop();
    emit uslugi.updateUI();
}


void ConnectionWindow::updateUI()
{
    const bool tryb_sieciowy = !uslugi.trybDzialania.isLocal();
    const bool tryb_arx = uslugi.trybDzialania.isSimmulationObject();
    ui->pushButton_connection->setEnabled(tryb_sieciowy);
    ui->lineEdit_wiadomosc->setEnabled(tryb_sieciowy);
    ui->pushButton_search->setEnabled(tryb_arx);
    ui->spinBox_ip1->setEnabled(tryb_arx);
    ui->spinBox_ip2->setEnabled(tryb_arx);
    ui->spinBox_ip3->setEnabled(tryb_arx);
    ui->spinBox_ip4->setEnabled(tryb_arx);
    ui->spinBox_port->setEnabled(tryb_sieciowy);
    ui->lineEdit_wiadomosc->setEnabled(tryb_sieciowy);
    ui->combo_znalezione->setEnabled(tryb_arx);
    ui->radioLokalny->setChecked(!tryb_sieciowy);
    ui->radioClient->setChecked(tryb_sieciowy && tryb_arx);
    ui->radioServer->setChecked(tryb_sieciowy && !tryb_arx);
    if(uslugi.authenticated.get())
        ui->pushButton_connection->setText(tryb_arx ? "Rozłącz" : "Stop serwera");
    else
        ui->pushButton_connection->setText(tryb_arx ? "Połącz" : "Start serwera");
}

void ConnectionWindow::onAuthChoiceRequired()
{
    QMessageBox msgBox;
    msgBox.setText("Urządzenie  chce się połączyć.");
    msgBox.setInformativeText("Wybierz tryb autoryzacji:");
    QPushButton *btnCode = msgBox.addButton("Generuj kod", QMessageBox::ActionRole);
    QPushButton *btnNoCode = msgBox.addButton("Bez kodu", QMessageBox::ActionRole);
    msgBox.addButton("Odrzuć", QMessageBox::RejectRole);
    msgBox.exec();

    if (msgBox.clickedButton() == btnCode)
    {
        int code = QRandomGenerator::global()->bounded(1000, 9999);
        log("Wygenerowano kod dla partnera: " + QString::number(code));
        uslugi.chooseAuthWithCode(code); // Tryb z kodem
    }
    else if (msgBox.clickedButton() == btnNoCode) { uslugi.chooseAuthWithoutCode(); }
    else { uslugi.chooseAuthReject(); }
}

void ConnectionWindow::onAuthError()
{
    QMessageBox::warning(this, "Błąd autoryzacji", QString("Podano błędny kod!"));
    onCodeEntry();
}

void ConnectionWindow::onCodeEntry()
{
    bool ok;
    QString text;
    better_luck_next_time:
    text = QInputDialog::getText(this, "Autoryzacja", "Partner wymaga kodu dostępu:", QLineEdit::Normal, "", &ok);

    if (ok && !text.isEmpty())
    {
        uslugi.authCodeVerification(text.toInt());
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
    if(uslugi.trybDzialania.isLocal())
        return;

    if (QMessageBox::question(this, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?") == QMessageBox::Yes)
    {
        uslugi.trybDzialania.set(WarstaUslug::TrybDzialania::LOCAL);
    }
    else
    {
        if(uslugi.trybDzialania.isSimmulationObject())
            ui->radioClient->setChecked(true);
        else
            ui->radioServer->setChecked(true);
    }
}


void ConnectionWindow::on_radioServer_clicked()
{
    uslugi.trybDzialania.set(WarstaUslug::TrybDzialania::NET_REG);
}


void ConnectionWindow::on_radioClient_clicked()
{
    uslugi.trybDzialania.set(WarstaUslug::TrybDzialania::NET_ARX);
}


void ConnectionWindow::on_pushButton_search_clicked()
{
    uslugi.searchDevices();
}


void ConnectionWindow::on_pushButton_connection_clicked()
{
    if(uslugi.authenticated.get())
    {
        int res = QMessageBox::question(this, "Rozłączanie", "Czy na pewno chcesz przerwać połączenie i wrócić do trybu stacjonarnego?");

        if (res == QMessageBox::Yes)
        {
            log("Rozłączanie ...");
            uslugi.disconnectGracefully();
        }
        return;
    }

    if(uslugi.trybDzialania.isSimmulationObject())
    {
        uslugi.connectAsClient(composeIPAddres(),
                               ui->spinBox_port->value());
    }
    else if(uslugi.trybDzialania.isSimmulationRegulator())
    {
        uslugi.startAsServer(ui->spinBox_port->value());
    }
}


void ConnectionWindow::on_combo_znalezione_currentTextChanged(const QString &arg1)
{
    decomposeIPAddres(arg1);
}

