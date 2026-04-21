#include "mainwindow.h"
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
    //Generator
    ui->amplituda->setValue(uslugi.generator.amplituda);
    ui->skladowaStala->setValue(uslugi.generator.skladowaStala);
    ui->wypelnienie->setValue(uslugi.generator.wypelnienie);
    bool sinus = false, prost = false;
    switch (uslugi.generator.typSygnalu)
    {
    case GeneratorWartosci::TypSygnalu::KWADRAT:
        prost = true;
        break;
    case GeneratorWartosci::TypSygnalu::SINUS:
        sinus = true;
        break;
    }
    ui->radioButton_prostokatny->setChecked(prost);
    ui->radioButton_sinusoidalny->setChecked(sinus);
    ui->czasTR->setValue(miliToSeconds(uslugi.generator.okres * uslugi.interwal));


    bool onoff = false, pid = false;
    switch (uslugi.Regulacja())
    {
    case UAR::RodzajSterowania::OnOff:
        onoff = true;
        break;
    case UAR::RodzajSterowania::PID:
        pid = true;
        break;
    }
    ui->radioButton_pid->setChecked(pid);
    ui->radioButton_onoff->setChecked(onoff);


    // RegulatorPID
    ui->spinBox_wzmocnienie->setValue(uslugi.pid.k);
    ui->stalaCalkowa->setValue(uslugi.pid.Ti);
    ui->stalaRozniczkowa->setValue(uslugi.pid.Td);

    bool pidZewn = false, pidWewn = false;
    switch (uslugi.pid.sposobLiczeniaCalki)
    {
    case RegulatorPID::Zewnetrzne:
        pidZewn = true;
        break;
    case RegulatorPID::Wewnetrzne:
        pidWewn = true;
        break;
    }
    ui->pidWewn->setChecked(pidWewn);
    ui->pidZewn->setChecked(pidZewn);
    ui->nasycenieMax->setValue(uslugi.pid.limityWyjscia.getMax());
    ui->nasycenieMin->setValue(uslugi.pid.limityWyjscia.getMin());
    ui->checkBox_nasycenie->setChecked(uslugi.pid.antiWindupActive);
    ui->checkBox_ograniczenie->setChecked(uslugi.pid.limityWyjscia.getActive());


    // Regulator OnOff
    ui->szerokoscHisterezy->setValue(uslugi.onOff.histereza);
    ui->wartoscSterowaniaON->setValue(uslugi.onOff.wartoscSterowania);

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
}


void MainWindow::on_spinBox_wzmocnienie_editingFinished()
{
    uslugi.pid.k = ui->spinBox_wzmocnienie->value();
}


void MainWindow::on_stalaCalkowa_editingFinished()
{
    uslugi.pid.Ti = ui->stalaCalkowa->value();
}


void MainWindow::on_stalaRozniczkowa_editingFinished()
{
    uslugi.pid.Td = ui->stalaRozniczkowa->value();
}


void MainWindow::on_resetPID_clicked()
{
    uslugi.pid.resetCzesciCalkujacej();
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
}


void MainWindow::on_amplituda_editingFinished()
{
    uslugi.generator.amplituda = ui->amplituda->value();
}


void MainWindow::on_skladowaStala_editingFinished()
{
    uslugi.generator.skladowaStala = ui->skladowaStala->value();
}


void MainWindow::on_wypelnienie_editingFinished()
{
    uslugi.generator.wypelnienie = ui->wypelnienie->value();
}


void MainWindow::on_radioButton_prostokatny_clicked()
{
    uslugi.generator.typSygnalu = GeneratorWartosci::TypSygnalu::KWADRAT;
}


void MainWindow::on_radioButton_sinusoidalny_clicked()
{
    uslugi.generator.typSygnalu = GeneratorWartosci::TypSygnalu::SINUS;
}



void MainWindow::on_nasycenieMax_editingFinished()
{
    uslugi.pid.limityWyjscia.setMax(ui->nasycenieMax->value());
}


void MainWindow::on_nasycenieMin_editingFinished()
{
    uslugi.pid.limityWyjscia.setMin(ui->nasycenieMin->value());
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
}


void MainWindow::on_checkBox_ograniczenie_clicked()
{
    uslugi.pid.limityWyjscia.setActive(ui->checkBox_ograniczenie->checkState() == Qt::CheckState::Checked);
}


void MainWindow::on_checkBox_nasycenie_clicked()
{
    uslugi.pid.antiWindupActive = ui->checkBox_nasycenie->checkState() == Qt::CheckState::Checked;
}

