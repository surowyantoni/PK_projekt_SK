#include "mainwindow.h"
#include "connectionwindow.h"
#include "parametryarx.h"
#include "plot.hpp"
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
    QObject::connect(&uslugi.netService, &NetService::simmulationRestart, this, [this](){
        pamiec_wykresow.clear();
    });


    ui->plot->deleteLater();

    ui->plot = new Plot(&pamiec_wykresow, this);
    ui->verticalLayout_plot->addWidget(ui->plot);
    ui->plot->update();

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
    ui->amplituda->setValue(uslugi.generator.amplituda.get());
    ui->skladowaStala->setValue(uslugi.generator.skladowaStala.get());
    ui->wypelnienie->setValue(uslugi.generator.wypelnienie.get());

    ui->radioButton_sinusoidalny->setChecked(uslugi.generator.typSygnalu.get() == GeneratorWartosci::TypSygnalu::SINUS);
    ui->radioButton_prostokatny->setChecked(uslugi.generator.typSygnalu.get() == GeneratorWartosci::TypSygnalu::KWADRAT);

    ui->czasTR->setValue(miliToSeconds(uslugi.generator.okres.get()));

    // --- Wybór Regulatora ---
    ui->radioButton_pid->setChecked(uslugi.regulacja.get() == UAR::RodzajSterowania::PID);
    ui->radioButton_onoff->setChecked(uslugi.regulacja.get() == UAR::RodzajSterowania::OnOff);

    // --- Regulator PID ---
    ui->spinBox_wzmocnienie->setValue(uslugi.pid.k.get());
    ui->stalaCalkowa->setValue(uslugi.pid.Ti.get());
    ui->stalaRozniczkowa->setValue(uslugi.pid.Td.get());

    ui->pidWewn->setChecked(uslugi.pid.sposobLiczeniaCalki.get() == RegulatorPID::Wewnetrzne);
    ui->pidZewn->setChecked(uslugi.pid.sposobLiczeniaCalki.get() == RegulatorPID::Zewnetrzne);

    ui->nasycenieMax->setValue(uslugi.pid.limityWyjscia.getMax());
    ui->nasycenieMin->setValue(uslugi.pid.limityWyjscia.getMin());
    ui->checkBox_nasycenie->setChecked(uslugi.pid.antiWindupActive.get());
    ui->checkBox_ograniczenie->setChecked(uslugi.pid.limityWyjscia.getActive());

    // --- Regulator OnOff ---
    ui->szerokoscHisterezy->setValue(uslugi.onOff.histereza.get());
    ui->wartoscSterowaniaON->setValue(uslugi.onOff.wartoscSterowania.get());

    if (paraARX)
       paraARX->refreshFromService();

    // BLOKOWANIE GUI ZALEŻNIE OD ROLI
    applyNetworkRoleBlocking();
}

void MainWindow::applyNetworkRoleBlocking()
{
    // Pobranie informacji o połączeniu z Warstwy Usług (poprzez NetService)
    bool isRegulator = uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_REG;
    bool isObject = uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_ARX;

    if (uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::LOCAL)
    {
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
    ui->groupBox_pid->setEnabled(isRegulator);
    ui->groupBox_onoff->setEnabled(isRegulator);
    ui->groupBox_regulacja->setEnabled(isRegulator);
    ui->groupBox_wykresy->setEnabled(true);
    ui->groupBox_generator->setEnabled(isRegulator);
    ui->groupBox_filtr->setEnabled(isRegulator);
    ui->pushButton_arx->setEnabled(isObject);
    ui->pushButton_startStop->setEnabled(true);
    ui->pushButton_reset->setEnabled(true);

}

void MainWindow::chartsUpdate(UAR::Tick tick, uint32_t czas)
{
    qDebug() << "CZas:" << czas << "P:" << tick.pid->P << "\tI:" << tick.pid->I << "\tD:" << tick.pid->D<< "\tUchyb:" << tick.uchyb << "\tREG:" << tick.wartoscRegulowana << "\tZAD::" << tick.wartoscZadana;
    auto punkt = std::make_pair(tick, czas);
    pamiec_wykresow.appendLastValue(punkt);
    const size_t ILE_PROBEK_MA_BYC_WIDOCZNYCH = ui->rozmiarWykresu->value() / miliToSeconds(uslugi.interwal.get());
    if(pamiec_wykresow.howManyPoints() >= ILE_PROBEK_MA_BYC_WIDOCZNYCH + 1)
    {
        if(pamiec_wykresow.howManyPoints() > ILE_PROBEK_MA_BYC_WIDOCZNYCH + 1)
            pamiec_wykresow.deleteFirstValue();
        pamiec_wykresow.deleteFirstValue();
    }
    ui->plot->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_startStop_clicked()
{
    uslugi.dziala.set(!uslugi.dziala.get());
}


void MainWindow::on_spinBox_wzmocnienie_editingFinished()
{
    uslugi.pid.k.set(ui->spinBox_wzmocnienie->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
        uslugi.netService.sendPidConfig();
}


void MainWindow::on_stalaCalkowa_editingFinished()
{
    uslugi.pid.Ti.set(ui->stalaCalkowa->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendPidConfig();
}


void MainWindow::on_stalaRozniczkowa_editingFinished()
{
    uslugi.pid.Td.set(ui->stalaRozniczkowa->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendPidConfig();
}


void MainWindow::on_resetPID_clicked()
{
    uslugi.pid.resetCzesciCalkujacej();
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendPidConfig();
}


void MainWindow::on_szerokoscHisterezy_editingFinished()
{
    uslugi.onOff.histereza.set(ui->szerokoscHisterezy->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendOnOffConfig();
}


void MainWindow::on_wartoscSterowaniaON_editingFinished()
{
    uslugi.onOff.wartoscSterowania.set(ui->wartoscSterowaniaON->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendOnOffConfig();
}


void MainWindow::on_radioButton_pid_clicked()
{
    uslugi.regulacja.set(UAR::RodzajSterowania::PID);
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendRegulationTypeConfig();
}


void MainWindow::on_radioButton_onoff_clicked()
{
    uslugi.regulacja.set(UAR::RodzajSterowania::OnOff);
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendRegulationTypeConfig();
}


void MainWindow::on_rozmiarWykresu_editingFinished()
{
    this->oknoObserwacji.set(secondsToMili(ui->rozmiarWykresu->value()));
}


void MainWindow::on_interwal_editingFinished()
{
    uslugi.interwal.set(ui->interwal->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendIntervalConfig();
}


void MainWindow::on_czasTR_editingFinished()
{
    uslugi.generator.okres.set(secondsToMili(ui->czasTR->value()));
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendGenConfig();
}


void MainWindow::on_amplituda_editingFinished()
{
    uslugi.generator.amplituda.set(ui->amplituda->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendGenConfig();
}


void MainWindow::on_skladowaStala_editingFinished()
{
    uslugi.generator.skladowaStala.set(ui->skladowaStala->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendGenConfig();
}


void MainWindow::on_wypelnienie_editingFinished()
{
    uslugi.generator.wypelnienie.set(ui->wypelnienie->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendGenConfig();
}


void MainWindow::on_radioButton_prostokatny_clicked()
{
    uslugi.generator.typSygnalu.set(GeneratorWartosci::TypSygnalu::KWADRAT);
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendGenConfig();
}


void MainWindow::on_radioButton_sinusoidalny_clicked()
{
    uslugi.generator.typSygnalu.set(GeneratorWartosci::TypSygnalu::SINUS);
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendGenConfig();
}

void MainWindow::on_nasycenieMax_editingFinished()
{
    uslugi.pid.limityWyjscia.setMax(ui->nasycenieMax->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendPidConfig();
}


void MainWindow::on_nasycenieMin_editingFinished()
{
    uslugi.pid.limityWyjscia.setMin(ui->nasycenieMin->value());
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendPidConfig();
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
    pamiec_wykresow.clear();
}


void MainWindow::on_checkBox_ograniczenie_clicked()
{
    uslugi.pid.limityWyjscia.setActive(ui->checkBox_ograniczenie->checkState() == Qt::CheckState::Checked);
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendPidConfig();
}


void MainWindow::on_checkBox_nasycenie_clicked()
{
    uslugi.pid.antiWindupActive.set(ui->checkBox_nasycenie->checkState() == Qt::CheckState::Checked);
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendPidConfig();
}

void MainWindow::on_actionPolacz_triggered()
{
    if (!m_connWindow)
        m_connWindow = new ConnectionWindow(&uslugi.netService, this);

    m_connWindow->show();
    m_connWindow->raise();
    m_connWindow->activateWindow();
}

void MainWindow::on_pidZewn_clicked()
{
    uslugi.pid.sposobLiczeniaCalki.set(RegulatorPID::Zewnetrzne);
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendPidConfig();
}

void MainWindow::on_pidWewn_clicked()
{
    uslugi.pid.sposobLiczeniaCalki.set(RegulatorPID::Wewnetrzne);
    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
    uslugi.netService.sendPidConfig();
}

