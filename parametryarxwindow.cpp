#include "parametryarxwindow.h"
#include "ui_mainwindow.h"
#include "ui_parametryarxwindow.h"
#include <vector>

ParametryARXWindow::ParametryARXWindow(WarstaUslug& uslugi)
    : QDialog()
    , wspolczynniki()
    , ui(new Ui::ParametryARXWindow)
    , uslugi(uslugi)
{
    ui->setupUi(this);
    updateUI();
    this->setWindowTitle("Parametry obiektu ARX");

    ui->spinBox_opoznienie->setMinimum(CONSTS::GUI::ARX::k_min);
    ui->spinBox_opoznienie->setMaximum(CONSTS::GUI::ARX::k_max);
    ui->spinBox_opoznienie->setSingleStep(CONSTS::GUI::ARX::k_step);

    ui->spinBox_zaklocenie->setMinimum(CONSTS::GUI::ARX::z_min);
    ui->spinBox_zaklocenie->setMaximum(CONSTS::GUI::ARX::z_max);
    ui->spinBox_zaklocenie->setSingleStep(CONSTS::GUI::ARX::z_step);

    ui->spinBox_wyjscieMax->setMinimum   (CONSTS::GUI::ARX::Y_min);
    ui->spinBox_wyjscieMax->setMaximum   (CONSTS::GUI::ARX::Y_max);
    ui->spinBox_wyjscieMax->setSingleStep(CONSTS::GUI::ARX::Y_step);

    ui->spinBox_wyjscieMin->setMinimum   (CONSTS::GUI::ARX::Y_min);
    ui->spinBox_wyjscieMin->setMaximum   (CONSTS::GUI::ARX::Y_max);
    ui->spinBox_wyjscieMin->setSingleStep(CONSTS::GUI::ARX::Y_step);

    ui->spinBox_wejscieMax->setMinimum   (CONSTS::GUI::ARX::U_min);
    ui->spinBox_wejscieMax->setMaximum   (CONSTS::GUI::ARX::U_max);
    ui->spinBox_wejscieMax->setSingleStep(CONSTS::GUI::ARX::U_step);

    ui->spinBox_wejscieMin->setMinimum   (CONSTS::GUI::ARX::U_min);
    ui->spinBox_wejscieMin->setMaximum   (CONSTS::GUI::ARX::U_max);
    ui->spinBox_wejscieMin->setSingleStep(CONSTS::GUI::ARX::U_step);

   }

void ParametryARXWindow::closeEvent(QCloseEvent* event)
{
    this->hide();
    emit closed();
}

void ParametryARXWindow::updateUI()
{
    for(auto& wspolczynnik : wspolczynniki)
    {
        wspolczynnik.first->hide();
        wspolczynnik.second->hide();
        wspolczynnik.first->deleteLater();
        wspolczynnik.second->deleteLater();
    }
    wspolczynniki.clear();
    for (auto& wspolczynniki : uslugi.arx.wspolczynniki.value)
    {
        dodajPareWspolczynnikow(wspolczynniki.A, wspolczynniki.B);
    }
    ui->spinBox_opoznienie->setValue(uslugi.arx.k.get());
    ui->spinBox_zaklocenie->setValue(uslugi.arx.z.get());

    ui->spinBox_wyjscieMax->setValue(uslugi.arx.limityRegulowana.getMax());
    ui->spinBox_wyjscieMin->setValue(uslugi.arx.limityRegulowana.getMin());
    ui->checkBoxOgraniczeniaWyjscia->setChecked(uslugi.arx.limityRegulowana.getActive());

    ui->spinBox_wejscieMax->setValue(uslugi.arx.limityZadana.getMax());
    ui->spinBox_wejscieMin->setValue(uslugi.arx.limityZadana.getMin());
    ui->checkBoxOgraniczeniaWejscia->setChecked(uslugi.arx.limityZadana.getActive());

    bool editable = !uslugi.trybDzialania.isSimmulationRegulator();

    for(auto& wspolczynnik : wspolczynniki)
    {
        wspolczynnik.first->setEnabled(editable);
        wspolczynnik.second->setEnabled(editable);
    }
    ui->spinBox_opoznienie->setEnabled(editable);
    ui->spinBox_zaklocenie->setEnabled(editable);

    ui->spinBox_wyjscieMax->setEnabled(editable);
    ui->spinBox_wyjscieMin->setEnabled(editable);
    ui->spinBox_wejscieMax->setEnabled(editable);
    ui->spinBox_wejscieMin->setEnabled(editable);
    ui->checkBoxOgraniczeniaWyjscia->setEnabled(editable);
    ui->checkBoxOgraniczeniaWejscia->setEnabled(editable);
    ui->pushButton_addWspolczynnik->setEnabled(editable);

}

ParametryARXWindow::~ParametryARXWindow()
{
    delete ui;
}

void ParametryARXWindow::on_buttonBox_accepted()
{
    std::vector<ARX::Wspolczynnik> vec;
    vec.reserve(wspolczynniki.size());
    for (auto& wspolczynnik : wspolczynniki)
    {
        vec.push_back({
        wspolczynnik.first->value(),
        wspolczynnik.second->value(),
        });
    }

    // Usuwamy zerowe współczynniki z końca, bo są niepotrzebne
    size_t to_remove = 0;
    for (size_t idx = vec.size() - 1; idx > 0; --idx)
    {
        if(vec[idx].A == 0.0 && vec[idx].B == 0.0)
            to_remove++;
        else
            break;
    }
    vec.resize(vec.size() - to_remove);

    auto arx = uslugi.arxChange();
    arx.instancja->wspolczynniki.value = vec;
    arx.instancja->k.set(ui->spinBox_opoznienie->value());
    arx.instancja->z.set(ui->spinBox_zaklocenie->value());
    arx.instancja->limityRegulowana.setMax(ui->spinBox_wyjscieMax->value());
    arx.instancja->limityRegulowana.setMin(ui->spinBox_wyjscieMin->value());
    arx.instancja->limityRegulowana.setActive(ui->checkBoxOgraniczeniaWyjscia->isChecked());
    arx.instancja->limityZadana.setMax(ui->spinBox_wejscieMax->value());
    arx.instancja->limityZadana.setMin(ui->spinBox_wejscieMin->value());
    arx.instancja->limityZadana.setActive(ui->checkBoxOgraniczeniaWejscia->isChecked());

    this->hide();
    emit closed();
}

void ParametryARXWindow::on_pushButton_addWspolczynnik_clicked()
{
    dodajPareWspolczynnikow();
}

void ParametryARXWindow::dodajPareWspolczynnikow(double a, double b)
{
    QDoubleSpinBox *spinBoxA = new QDoubleSpinBox(this);
    QDoubleSpinBox *spinBoxB = new QDoubleSpinBox(this);

    spinBoxA->setMaximum(CONSTS::GUI::ARX::A_max);
    spinBoxA->setMinimum(CONSTS::GUI::ARX::A_min);
    spinBoxA->setSingleStep(CONSTS::GUI::ARX::A_step);
    spinBoxA->setValue(a);

    spinBoxB->setMaximum(CONSTS::GUI::ARX::B_max);
    spinBoxB->setMinimum(CONSTS::GUI::ARX::B_min);
    spinBoxB->setSingleStep(CONSTS::GUI::ARX::B_step);
    spinBoxB->setValue(b);

    wspolczynniki.append(std::make_pair(spinBoxA, spinBoxB));

    ui->verticalLayout_A->addWidget(spinBoxA);
    ui->verticalLayout_B->addWidget(spinBoxB);
}


void ParametryARXWindow::on_buttonBox_rejected()
{
    emit closed();
    this->hide();
}

