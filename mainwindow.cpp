#include "mainwindow.h"
#include "connectionwindow.h"
#include "parametryarxwindow.h"
#include "plot.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , oknoObserwacji(secondsToMili(10.0))
    , ui(new Ui::MainWindow)
    , uslugi(WarstaUslug())
    , parameters_arx_window(uslugi)
    , connection_window(ConnectionWindow(uslugi, this))
{
    ui->setupUi(this);
    externalUIUpdate();
    QObject::connect(&uslugi, &WarstaUslug::updateCharts, this, &MainWindow::chartsUpdate);
    QObject::connect(&uslugi, &WarstaUslug::updateUI, this, &MainWindow::externalUIUpdate);
    QObject::connect(&uslugi.netService, &NetService::simmulationRestart, this, [this](){
        uslugi.reset();
        pamiec_wykresow.clear();
    });
    QObject::connect(&parameters_arx_window, &ParametryARXWindow::closed, this, [this](){
        ui->pushButton_arx->setEnabled(true);
    });


    // Podmieniamy widget na własną klase Plot
    ui->plot->deleteLater();
    ui->plot = new Plot(&pamiec_wykresow, this);
    ui->verticalLayout_plot->addWidget(ui->plot);
    ui->plot->update();

    //Ustawianie parametrow kontrolek w GUI
    ui->spinBox_amplituda->setMaximum(CONSTS::GUI::Generator::amplituda_max);
    ui->spinBox_amplituda->setMinimum(CONSTS::GUI::Generator::amplituda_min);
    ui->spinBox_amplituda->setSingleStep(CONSTS::GUI::Generator::amplituda_step);

    ui->spinBox_okres->setMaximum   (CONSTS::GUI::Generator::okres_max);
    ui->spinBox_okres->setMinimum   (CONSTS::GUI::Generator::okres_min);
    ui->spinBox_okres->setSingleStep(CONSTS::GUI::Generator::okres_step);

    ui->spinBox_skladowaStala->setMaximum   (CONSTS::GUI::Generator::skladowaStala_max);
    ui->spinBox_skladowaStala->setMinimum   (CONSTS::GUI::Generator::skladowaStala_min);
    ui->spinBox_skladowaStala->setSingleStep(CONSTS::GUI::Generator::skladowaStala_step);

    ui->spinBox_wypelnienie->setMaximum   (CONSTS::GUI::Generator::wypelnienie_max);
    ui->spinBox_wypelnienie->setMinimum   (CONSTS::GUI::Generator::wypelnienie_min);
    ui->spinBox_wypelnienie->setSingleStep(CONSTS::GUI::Generator::wypelnienie_step);

    ui->spinBox_wzmocnienie->setMaximum   (CONSTS::GUI::PID::P_max);
    ui->spinBox_wzmocnienie->setMinimum   (CONSTS::GUI::PID::P_min);
    ui->spinBox_wzmocnienie->setSingleStep(CONSTS::GUI::PID::P_step);

    ui->spinBox_stalaCalkowa->setMaximum   (CONSTS::GUI::PID::I_max);
    ui->spinBox_stalaCalkowa->setMinimum   (CONSTS::GUI::PID::I_min);
    ui->spinBox_stalaCalkowa->setSingleStep(CONSTS::GUI::PID::I_step);

    ui->spinBox_stalaRozniczkowa->setMaximum   (CONSTS::GUI::PID::D_max);
    ui->spinBox_stalaRozniczkowa->setMinimum   (CONSTS::GUI::PID::D_min);
    ui->spinBox_stalaRozniczkowa->setSingleStep(CONSTS::GUI::PID::D_step);

    ui->spinBox_sterowanieMax->setMaximum   (CONSTS::GUI::PID::U_max);
    ui->spinBox_sterowanieMax->setMinimum   (CONSTS::GUI::PID::U_min);
    ui->spinBox_sterowanieMax->setSingleStep(CONSTS::GUI::PID::U_step);
    ui->spinBox_sterowanieMin->setMaximum   (CONSTS::GUI::PID::U_max);
    ui->spinBox_sterowanieMin->setMinimum   (CONSTS::GUI::PID::U_min);
    ui->spinBox_sterowanieMin->setSingleStep(CONSTS::GUI::PID::U_step);

    ui->spinBox_wartoscSterowaniaON->setMaximum   (CONSTS::GUI::OnOff::sterowanie_max);
    ui->spinBox_wartoscSterowaniaON->setMinimum   (CONSTS::GUI::OnOff::sterowanie_min);
    ui->spinBox_wartoscSterowaniaON->setSingleStep(CONSTS::GUI::OnOff::sterowanie_step);

    ui->spinBox_szerokoscHisterezy->setMaximum   (CONSTS::GUI::OnOff::histereza_max);
    ui->spinBox_szerokoscHisterezy->setMinimum   (CONSTS::GUI::OnOff::histereza_min);
    ui->spinBox_szerokoscHisterezy->setSingleStep(CONSTS::GUI::OnOff::histereza_step);

    ui->spinBox_interwal->setMaximum   (CONSTS::GUI::UAR::interwal_max);
    ui->spinBox_interwal->setMinimum   (CONSTS::GUI::UAR::interwal_min);
    ui->spinBox_interwal->setSingleStep(CONSTS::GUI::UAR::interwal_step);

    ui->spinBox_rozmiarWykresu->setMaximum   (CONSTS::GUI::oknoObserwacji_max);
    ui->spinBox_rozmiarWykresu->setMinimum   (CONSTS::GUI::oknoObserwacji_min);
    ui->spinBox_rozmiarWykresu->setSingleStep(CONSTS::GUI::oknoObserwacji_step);
    ui->spinBox_rozmiarWykresu->setValue(CONSTS::GUI::oknoObserwacji);

}
void MainWindow::externalUIUpdate()
{
    // BLOKADA SYGNAŁÓW (pętla sieciowa)
    const QSignalBlocker bAmp(ui->spinBox_amplituda);
    const QSignalBlocker bOff(ui->spinBox_skladowaStala);
    const QSignalBlocker bDuty(ui->spinBox_wypelnienie);
    const QSignalBlocker bPer(ui->spinBox_okres);
    const QSignalBlocker bP(ui->spinBox_wzmocnienie);
    const QSignalBlocker bI(ui->spinBox_stalaCalkowa);
    const QSignalBlocker bD(ui->spinBox_stalaRozniczkowa);
    const QSignalBlocker bMin(ui->spinBox_sterowanieMin);
    const QSignalBlocker bMax(ui->spinBox_sterowanieMax);
    const QSignalBlocker bHyst(ui->spinBox_szerokoscHisterezy);
    const QSignalBlocker bOnOffU(ui->spinBox_wartoscSterowaniaON);


    ui->pushButton_startStop->setText(uslugi.dziala.get() ? "STOP" : "START");

    // --- Generator ---
    ui->spinBox_amplituda->setValue(uslugi.generator.amplituda.get());
    ui->spinBox_skladowaStala->setValue(uslugi.generator.skladowaStala.get());
    ui->spinBox_wypelnienie->setValue(uslugi.generator.wypelnienie.get());
    ui->spinBox_okres->setValue(miliToSeconds(uslugi.generator.okres.get()));
    ui->radioButton_sinusoidalny->setChecked(uslugi.generator.typSygnalu.get() == GeneratorWartosci::TypSygnalu::SINUS);
    ui->radioButton_prostokatny->setChecked(uslugi.generator.typSygnalu.get() == GeneratorWartosci::TypSygnalu::KWADRAT);


    // --- Wybór Regulatora ---
    ui->radioButton_pid->setChecked(uslugi.regulacja.get() == UAR::RodzajSterowania::PID);
    ui->radioButton_onoff->setChecked(uslugi.regulacja.get() == UAR::RodzajSterowania::OnOff);

    // --- Regulator PID ---
    ui->spinBox_wzmocnienie->setValue(uslugi.pid.k.get());
    ui->spinBox_stalaCalkowa->setValue(uslugi.pid.Ti.get());
    ui->spinBox_stalaRozniczkowa->setValue(uslugi.pid.Td.get());

    ui->radioButton_pidWewn->setChecked(uslugi.pid.sposobLiczeniaCalki.get() == RegulatorPID::Wewnetrzne);
    ui->radioButton_pidZewn->setChecked(uslugi.pid.sposobLiczeniaCalki.get() == RegulatorPID::Zewnetrzne);

    ui->spinBox_sterowanieMax->setValue(uslugi.pid.limityWyjscia.getMax());
    ui->spinBox_sterowanieMin->setValue(uslugi.pid.limityWyjscia.getMin());
    ui->checkBox_nasycenie->setChecked(uslugi.pid.antiWindupActive.get());
    ui->checkBox_ograniczenie->setChecked(uslugi.pid.limityWyjscia.getActive());

    // --- Regulator OnOff ---
    ui->spinBox_szerokoscHisterezy->setValue(uslugi.onOff.histereza.get());
    ui->spinBox_wartoscSterowaniaON->setValue(uslugi.onOff.wartoscSterowania.get());

    ui->spinBox_interwal->setValue(uslugi.interwal.get());

    if (parameters_arx_window.isVisible())
       parameters_arx_window.updateUI();

    // BLOKOWANIE GUI ZALEŻNIE OD ROLI
    // Pobranie informacji o połączeniu z Warstwy Usług (poprzez NetService)
    bool isRegulator = uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_REG;
    bool isObject = uslugi.trybDzialania.get() == WarstaUslug::TrybDzialania::NET_ARX;
    bool isConnected = uslugi.netService.isAuthenticated();

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
    ui->pushButton_startStop->setEnabled(isConnected);
    ui->pushButton_reset->setEnabled(isConnected);
}

void MainWindow::chartsUpdate(UAR::Tick tick, WarstaUslug::Czas czas)
{
#ifdef DEBUG
    QString out = "Czas:" + QString::number(czas) + "ms";
    if(tick.pid.has_value())
        out += "  P:" + QString::number(tick.pid->P) + "  I:" + QString::number(tick.pid->I) + "  D:" + QString::number(tick.pid->D);
    out += "  Uchyb:" + QString::number(tick.uchyb);
    out += "  REG:" + QString::number(tick.wartoscRegulowana);
    out += "  ZAD:" + QString::number(tick.wartoscZadana);
    qDebug() << out;
    ui->label_rzeczytistyTick->setText( "Prawdziwy czas między tickami: " + QString::number(uslugi.measuredInterval.get()));
    if(uslugi.measuredInterval.get() > uslugi.interwal.get()  + ((Plot*)ui->plot)->refreshInterval.get())
    {
        qDebug() << "Nie wyrabia kurła";
    }
#endif
    auto punkt = std::make_pair(tick, czas);
    pamiec_wykresow.appendLastValue(punkt);
    if(pamiec_wykresow.timeWidth() > oknoObserwacji.get())
    {
        if(pamiec_wykresow.timeWidth() > oknoObserwacji.get())
            pamiec_wykresow.deleteFirstValue();
        pamiec_wykresow.deleteFirstValue();
    }
    if constexpr (!CONSTS::PLOTS::UPDATE_ON_TICK)
    {
        ui->plot->update();
    }
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
    uslugi.pidChange().instancja->k.set(ui->spinBox_wzmocnienie->value());
}


void MainWindow::on_radioButton_pid_clicked()
{
    uslugi.regulacja.set(UAR::RodzajSterowania::PID);
}


void MainWindow::on_radioButton_onoff_clicked()
{
    uslugi.regulacja.set(UAR::RodzajSterowania::OnOff);
}


void MainWindow::on_radioButton_prostokatny_clicked()
{
    uslugi.generatorChange().instancja->typSygnalu.set(GeneratorWartosci::TypSygnalu::KWADRAT);
}


void MainWindow::on_radioButton_sinusoidalny_clicked()
{
    uslugi.generatorChange().instancja->typSygnalu.set(GeneratorWartosci::TypSygnalu::SINUS);
}


void MainWindow::on_pushButton_arx_clicked()
{
    parameters_arx_window.updateUI();
    ui->pushButton_arx->setEnabled(false);
    parameters_arx_window.show();
}


void MainWindow::on_pushButton_reset_clicked()
{
    uslugi.reset();
    pamiec_wykresow.clear();
}


void MainWindow::on_checkBox_ograniczenie_clicked()
{
    uslugi.pidChange().instancja->limityWyjscia.setActive(ui->checkBox_ograniczenie->checkState() == Qt::CheckState::Checked);
}


void MainWindow::on_checkBox_nasycenie_clicked()
{
    uslugi.pidChange().instancja->antiWindupActive.set(ui->checkBox_nasycenie->checkState() == Qt::CheckState::Checked);
}

void MainWindow::on_actionPolacz_triggered()
{
    connection_window.show();
    connection_window.raise();
    connection_window.activateWindow();
}

void MainWindow::on_horizontalSlider_wypelnienie_valueChanged(int value)
{
    ui->spinBox_wypelnienie->setValue((double)value / ui->horizontalSlider_wypelnienie->maximum());
    on_spinBox_wypelnienie_editingFinished();
}


void MainWindow::on_spinBox_stalaCalkowa_editingFinished()
{
    uslugi.pidChange().instancja->Ti.set(ui->spinBox_stalaCalkowa->value());
}


void MainWindow::on_spinBox_stalaRozniczkowa_editingFinished()
{
    uslugi.pidChange().instancja->Td.set(ui->spinBox_stalaRozniczkowa->value());
}


void MainWindow::on_pushButton_resetPID_clicked()
{
    uslugi.pidChange().instancja->resetCzesciCalkujacej();
}


void MainWindow::on_radioButton_pidWewn_clicked()
{
     uslugi.pidChange().instancja->sposobLiczeniaCalki.set(RegulatorPID::Wewnetrzne);
}


void MainWindow::on_radioButton_pidZewn_clicked()
{
    uslugi.pidChange().instancja->sposobLiczeniaCalki.set(RegulatorPID::Zewnetrzne);
}


void MainWindow::on_spinBox_szerokoscHisterezy_editingFinished()
{
     uslugi.onOffChange().instancja->histereza.set(ui->spinBox_szerokoscHisterezy->value());
}


void MainWindow::on_spinBox_wartoscSterowaniaON_editingFinished()
{
     uslugi.onOffChange().instancja->wartoscSterowania.set(ui->spinBox_wartoscSterowaniaON->value());
}


void MainWindow::on_spinBox_rozmiarWykresu_editingFinished()
{
    this->oknoObserwacji.set(secondsToMili(ui->spinBox_rozmiarWykresu->value()));
}


void MainWindow::on_spinBox_interwal_editingFinished()
{
    uslugi.interwal.set(ui->spinBox_interwal->value());
}


void MainWindow::on_spinBox_okres_editingFinished()
{
    uslugi.generatorChange().instancja->okres.set(secondsToMili(ui->spinBox_okres->value()));
}


void MainWindow::on_spinBox_amplituda_editingFinished()
{
     uslugi.generatorChange().instancja->amplituda.set(ui->spinBox_amplituda->value());
}


void MainWindow::on_spinBox_skladowaStala_editingFinished()
{
    uslugi.generatorChange().instancja->skladowaStala.set(ui->spinBox_skladowaStala->value());
}


void MainWindow::on_spinBox_wypelnienie_editingFinished()
{
    ui->horizontalSlider_wypelnienie->setValue(ui->spinBox_wypelnienie->value() * ui->horizontalSlider_wypelnienie->maximum());
    uslugi.generatorChange().instancja->wypelnienie.set(ui->spinBox_wypelnienie->value());
}



void MainWindow::on_spinBox_sterowanieMin_editingFinished()
{
    uslugi.pidChange().instancja->limityWyjscia.setMin(ui->spinBox_sterowanieMin->value());
}


void MainWindow::on_spinBox_sterowanieMax_editingFinished()
{
     uslugi.pidChange().instancja->limityWyjscia.setMax(ui->spinBox_sterowanieMax->value());
}

