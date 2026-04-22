#include "mainwindow.h"
#include "connectionwindow.h"
#include "parametryarx.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , oknoObserwacji(secondsToMili(10.0))
    , ui(new Ui::MainWindow)
    , uslugi(WarstaUslug())
{
    ui->setupUi(this);
    externalUIUpdate();
    QObject::connect(&uslugi, &WarstaUslug::updateCharts, this, &MainWindow::chartsUpdate);
    QObject::connect(&uslugi, &WarstaUslug::updateUI, this, &MainWindow::externalUIUpdate);
    ui->plot_zadana_regulowana->serie.push_back(Seria{&zadana, Qt::red, "Zadana"});
    ui->plot_zadana_regulowana->serie.push_back(Seria{&regulowana, Qt::blue, "Regulowana"});
    ui->plot_uchyb->serie.push_back(Seria{&uchyb, Qt::GlobalColor::darkRed, "Uchyb"});
    ui->plot_sterowanie->serie.push_back(Seria{&sterowanie, Qt::GlobalColor::green, "Sterowanie"});
    ui->plot_pid->serie.push_back(Seria{&pidP, Qt::GlobalColor::green, "Czlon Proporcjonalny"});
    ui->plot_pid->serie.push_back(Seria{&pidI, Qt::GlobalColor::red, "Czlon Całkujacy"});
    ui->plot_pid->serie.push_back(Seria{&pidD, Qt::GlobalColor::blue, "Czlon Rożniczkujący"});
    ui->plot_zadana_regulowana->update();
    ui->plot_uchyb->update();
    ui->plot_sterowanie->update();
    ui->plot_pid->update();

}
void MainWindow::externalUIUpdate()
{
    // BLOKADA SYGNAŁÓW (pętla sieciowa)
    const QSignalBlocker bAmp(ui->amplituda);
    const QSignalBlocker bOff(ui->skladowaStala);
    const QSignalBlocker bDuty(ui->wypelnienie);
    const QSignalBlocker bPer(ui->czasTR);
    const QSignalBlocker bP(ui->spinBox_wzmocnienie);
    const QSignalBlocker bI(ui->stalaCalkowa);
    const QSignalBlocker bD(ui->stalaRozniczkowa);
    const QSignalBlocker bMin(ui->nasycenieMin);
    const QSignalBlocker bMax(ui->nasycenieMax);
    const QSignalBlocker bHyst(ui->szerokoscHisterezy);
    const QSignalBlocker bOnOffU(ui->wartoscSterowaniaON);

    // --- Generator ---
    ui->amplituda->setValue(uslugi.generator.amplituda);
    ui->skladowaStala->setValue(uslugi.generator.skladowaStala);
    ui->wypelnienie->setValue(uslugi.generator.wypelnienie);

    ui->radioButton_sinusoidalny->setChecked(uslugi.generator.typSygnalu == GeneratorWartosci::TypSygnalu::SINUS);
    ui->radioButton_prostokatny->setChecked(uslugi.generator.typSygnalu == GeneratorWartosci::TypSygnalu::KWADRAT);

    ui->czasTR->setValue((double)(uslugi.generator.okres * uslugi.interwal) / 1000.0);

    // --- Wybór Regulatora ---
    ui->radioButton_pid->setChecked(uslugi.Regulacja() == UAR::RodzajSterowania::PID);
    ui->radioButton_onoff->setChecked(uslugi.Regulacja() == UAR::RodzajSterowania::OnOff);

    // --- Regulator PID ---
    ui->spinBox_wzmocnienie->setValue(uslugi.pid.k);
    ui->stalaCalkowa->setValue(uslugi.pid.Ti);
    ui->stalaRozniczkowa->setValue(uslugi.pid.Td);

    ui->pidWewn->setChecked(uslugi.pid.sposobLiczeniaCalki == RegulatorPID::Wewnetrzne);
    ui->pidZewn->setChecked(uslugi.pid.sposobLiczeniaCalki == RegulatorPID::Zewnetrzne);

    ui->nasycenieMax->setValue(uslugi.pid.limityWyjscia.getMax());
    ui->nasycenieMin->setValue(uslugi.pid.limityWyjscia.getMin());
    ui->checkBox_nasycenie->setChecked(uslugi.pid.antiWindupActive);
    ui->checkBox_ograniczenie->setChecked(uslugi.pid.limityWyjscia.getActive());

    // --- Regulator OnOff ---
    ui->szerokoscHisterezy->setValue(uslugi.onOff.histereza);
    ui->wartoscSterowaniaON->setValue(uslugi.onOff.wartoscSterowania);

    if (paraARX)
       paraARX->refreshFromService();

    // BLOKOWANIE GUI ZALEŻNIE OD ROLI
    applyNetworkRoleBlocking();
}

void MainWindow::applyNetworkRoleBlocking()
{
    // Pobranie informacji o połączeniu z Warstwy Usług (poprzez NetService)
    bool connected = uslugi.getNetService() && uslugi.getNetService()->isConnected();
    bool isServer = uslugi.getNetService() && uslugi.getNetService()->isServer();
    bool isClient = uslugi.getNetService() && uslugi.getNetService()->isClient();

    if (!connected) {
        // TRYB STACJONARNY: Wszystko odblokowane
        ui->groupBox_pid->setEnabled(true);
        ui->groupBox_onoff->setEnabled(true);
        ui->groupBox_regulacja->setEnabled(true);
        ui->groupBox_wykresy->setEnabled(true);
        ui->groupBox_generator->setEnabled(true);
        ui->groupBox_filtr->setEnabled(true);
        ui->pushButton_arx->setEnabled(true);
        ui->pushButton_startStop->setEnabled(true);
        ui->pushButton_reset->setEnabled(true);
        return;
    }


    // ROLA: sieciowa
    ui->groupBox_pid->setEnabled(isServer);
    ui->groupBox_onoff->setEnabled(isServer);
    ui->groupBox_regulacja->setEnabled(isServer);
    ui->groupBox_wykresy->setEnabled(true);
    ui->groupBox_generator->setEnabled(isServer);
    ui->groupBox_filtr->setEnabled(isServer);
    ui->pushButton_arx->setEnabled(isClient);
    ui->pushButton_startStop->setEnabled(true);
    ui->pushButton_reset->setEnabled(true);

}

void MainWindow::chartsUpdate(UAR::Tick tick, uint32_t czas)
{
    // qDebug() << "CZas:" << czas << "TICK:" << tick.wartoscZadana;
    zadana.appendLastValue(QPointF(miliToSeconds(czas), tick.wartoscZadana));
    regulowana.appendLastValue(QPointF(miliToSeconds(czas), tick.wartoscRegulowana));
    sterowanie.appendLastValue(QPointF(miliToSeconds(czas), tick.sterowanie));
    uchyb.appendLastValue(QPointF(miliToSeconds(czas), tick.uchyb));
    if(tick.pid.has_value())
    {
        pidP.appendLastValue(QPointF(miliToSeconds(czas), tick.pid->P));
        pidI.appendLastValue(QPointF(miliToSeconds(czas), tick.pid->I));
        pidD.appendLastValue(QPointF(miliToSeconds(czas), tick.pid->D));
    }

    ui->plot_zadana_regulowana->update();
    ui->plot_uchyb->update();
    ui->plot_sterowanie->update();
    ui->plot_pid->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_startStop_clicked()
{
    uslugi.dziala = !uslugi.dziala;

    if (uslugi.getNetService() && uslugi.getNetService()->isConnected())
    {
        int cmd = uslugi.dziala ? 1 : 0;
        uslugi.getNetService()->sendSimCmd(cmd);
    }
}


void MainWindow::on_spinBox_wzmocnienie_editingFinished()
{
    uslugi.pid.k = ui->spinBox_wzmocnienie->value();

    syncPidToNetwork();
}


void MainWindow::on_stalaCalkowa_editingFinished()
{
    uslugi.pid.Ti = ui->stalaCalkowa->value();

    syncPidToNetwork();
}


void MainWindow::on_stalaRozniczkowa_editingFinished()
{
    uslugi.pid.Td = ui->stalaRozniczkowa->value();

    syncPidToNetwork();
}


void MainWindow::on_resetPID_clicked()
{
    uslugi.pid.resetCzesciCalkujacej();

    syncPidToNetwork();
}


void MainWindow::on_szerokoscHisterezy_editingFinished()
{
    uslugi.onOff.histereza = ui->szerokoscHisterezy->value();
}


void MainWindow::on_wartoscSterowaniaON_editingFinished()
{
    uslugi.onOff.wartoscSterowania = ui->wartoscSterowaniaON->value();
}


void MainWindow::on_radioButton_pid_clicked()
{
    uslugi.Regulacja(UAR::RodzajSterowania::PID);
}


void MainWindow::on_radioButton_onoff_clicked()
{
    uslugi.Regulacja(UAR::RodzajSterowania::OnOff);
}


void MainWindow::on_rozmiarWykresu_editingFinished()
{
    this->oknoObserwacji = secondsToMili(ui->rozmiarWykresu->value());
}


void MainWindow::on_interwal_editingFinished()
{
    uslugi.interwal = ui->interwal->value();
    uslugi.generator.okres = secondsToMili(ui->czasTR->value()) / uslugi.interwal;

}


void MainWindow::on_czasTR_editingFinished()
{
    uslugi.generator.okres = secondsToMili(ui->czasTR->value()) / uslugi.interwal;
    syncGenToNetwork();
}


void MainWindow::on_amplituda_editingFinished()
{
    uslugi.generator.amplituda = ui->amplituda->value();
    syncGenToNetwork();
}


void MainWindow::on_skladowaStala_editingFinished()
{
    uslugi.generator.skladowaStala = ui->skladowaStala->value();
    syncGenToNetwork();
}


void MainWindow::on_wypelnienie_editingFinished()
{
    uslugi.generator.wypelnienie = ui->wypelnienie->value();
    syncGenToNetwork();
}


void MainWindow::on_radioButton_prostokatny_clicked()
{
    uslugi.generator.typSygnalu = GeneratorWartosci::TypSygnalu::KWADRAT;
    syncGenToNetwork();
}


void MainWindow::on_radioButton_sinusoidalny_clicked()
{
    uslugi.generator.typSygnalu = GeneratorWartosci::TypSygnalu::SINUS;
    syncGenToNetwork();
}

void MainWindow::on_nasycenieMax_editingFinished()
{
    uslugi.pid.limityWyjscia.setMax(ui->nasycenieMax->value());
    syncPidToNetwork();
}


void MainWindow::on_nasycenieMin_editingFinished()
{
    uslugi.pid.limityWyjscia.setMin(ui->nasycenieMin->value());
    syncPidToNetwork();
}


void MainWindow::on_pushButton_arx_clicked()
{
    paraARX = new ParametryARX(this);
    paraARX->show();
    ui->pushButton_arx->setEnabled(false);
}


void MainWindow::on_pushButton_reset_clicked()
{
    uslugi.reset();
    zadana.clear();
    regulowana.clear();
    pidP.clear();
    pidI.clear();
    pidD.clear();
    uchyb.clear();
    sterowanie.clear();

    if (uslugi.getNetService() && uslugi.getNetService()->isConnected())
        uslugi.getNetService()->sendSimCmd(2);
}


void MainWindow::on_checkBox_ograniczenie_clicked()
{
    uslugi.pid.limityWyjscia.setActive(ui->checkBox_ograniczenie->checkState() == Qt::CheckState::Checked);
    syncPidToNetwork();
}


void MainWindow::on_checkBox_nasycenie_clicked()
{
    uslugi.pid.antiWindupActive = ui->checkBox_nasycenie->checkState() == Qt::CheckState::Checked;
    syncPidToNetwork();
}

void MainWindow::on_actionPolacz_triggered()
{
    if (!m_connWindow)
        m_connWindow = new ConnectionWindow(uslugi.getNetService(), this);

    m_connWindow->show();
    m_connWindow->raise();
    m_connWindow->activateWindow();
}

void MainWindow::on_pidZewn_clicked()
{
    uslugi.pid.sposobLiczeniaCalki = RegulatorPID::Zewnetrzne;
    syncPidToNetwork();
}

void MainWindow::on_pidWewn_clicked()
{
    uslugi.pid.sposobLiczeniaCalki = RegulatorPID::Wewnetrzne;
    syncPidToNetwork();
}

void MainWindow::syncPidToNetwork()
{
    if (uslugi.getNetService() && uslugi.getNetService()->isConnected())
        uslugi.getNetService()->sendPidConfig(uslugi.pid.k, uslugi.pid.Ti, uslugi.pid.Td, static_cast<int>(uslugi.pid.sposobLiczeniaCalki.get()), ui->nasycenieMin->value(), ui->nasycenieMax->value());
}

void MainWindow::syncGenToNetwork()
{
    if (uslugi.getNetService() && uslugi.getNetService()->isConnected())
    {
        int typ = static_cast<int>(uslugi.generator.typSygnalu.get());
        double amp = uslugi.generator.amplituda;
        double offset = uslugi.generator.skladowaStala;
        double duty = uslugi.generator.wypelnienie;
        double periodSec = uslugi.generator.okres;

        uslugi.getNetService()->sendGenConfig(typ, amp, periodSec, offset, duty);
    }
}

void MainWindow::syncARXToNetwork()
{
    if (uslugi.getNetService() && uslugi.getNetService()->isConnected())
    {
        QVector<double> vecA, vecB;

        for (const auto& w : uslugi.arx.wspolczynniki.get())
        {
            vecA.append(w.A);
            vecB.append(w.B);
        }

        uslugi.getNetService()->sendArxConfig(vecA, vecB, uslugi.arx.k, uslugi.arx.z, uslugi.arx.limityZadana.getMin(), uslugi.arx.limityZadana.getMax(),
                        uslugi.arx.limityRegulowana.getMin(), uslugi.arx.limityRegulowana.getMax());
    }
}

