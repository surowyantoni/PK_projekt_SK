#include "parametryarxwindow.h"
#include "ui_mainwindow.h"
#include "ui_parametryarxwindow.h"
#include <vector>

ParametryARXWindow::ParametryARXWindow(WarstaUslug& uslugi)
    : QDialog()
    , ui(new Ui::ParametryARXWindow)
    , uslugi(uslugi)
{
    ui->setupUi(this);

    new QVBoxLayout(ui->verticalFrame);
    new QVBoxLayout(ui->verticalFrame_2);

    dynamicLayoutVectorA = qobject_cast<QVBoxLayout *>(ui->verticalFrame->layout());
    dynamicLayoutVectorB = qobject_cast<QVBoxLayout *>(ui->verticalFrame_2->layout());

    if (!dynamicLayoutVectorA) {
        dynamicLayoutVectorA = new QVBoxLayout(ui->verticalFrame);
        ui->verticalFrame->setLayout(dynamicLayoutVectorA);
    }
    if (!dynamicLayoutVectorB) {
        dynamicLayoutVectorB = new QVBoxLayout(ui->verticalFrame_2);
        ui->verticalFrame_2->setLayout(dynamicLayoutVectorB);
    }

    std::vector<ARX::Wspolczynnik> vec = uslugi.arx.wspolczynniki.value;

    if (vec.empty())
    {
        for (int i = 0; i < 3; ++i)
        {
            addNewFieldVectorA(0.0);
            addNewFieldVectorB(0.0);
        }
    } else
    {
        for (auto val : vec)
        {
            addNewFieldVectorA(val.A);
            addNewFieldVectorB(val.B);
        }
    }

    ui->opoznienie->setValue(uslugi.arx.k.get());

    ui->szum->setValue(uslugi.arx.z.get());
    ui->checkboxOgraniczenia->setChecked(uslugi.arx.limityZadana.getActive());

    ui->odWartoscSterowania->setValue(uslugi.arx.limityZadana.getMin());
    ui->doWartoscSterowania->setValue(uslugi.arx.limityZadana.getMax());
}

// Zmodyfikowana funkcja dodająca pole z wartością
void ParametryARXWindow::addNewFieldVectorA(double value)
{
    QHBoxLayout *rowLayout = new QHBoxLayout();
    QDoubleSpinBox *newSpinBox = new QDoubleSpinBox(this);
    newSpinBox->setMinimum(-99.990000);
    newSpinBox->setSingleStep(0.1);
    newSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // Tuta wpisujemy wartość do okienka
    newSpinBox->setValue(value);

    QLabel *newLabel = new QLabel(this);
    newLabel->setFixedWidth(20);
    int fieldIndex = dynamicLayoutVectorA->count() + 1;
    newLabel->setText(QString("%1:").arg(fieldIndex));

    rowLayout->addWidget(newLabel);
    rowLayout->addWidget(newSpinBox);
    dynamicLayoutVectorA->addLayout(rowLayout);
}

// Zmodyfikowana funkcja dodająca pole z wartością dla B
void ParametryARXWindow::addNewFieldVectorB(double value)
{
    QHBoxLayout *rowLayout = new QHBoxLayout();
    QDoubleSpinBox *newSpinBox = new QDoubleSpinBox(this);
    newSpinBox->setMinimum(-99.990000);
    newSpinBox->setSingleStep(0.1);
    newSpinBox->setValue(value);

    QLabel *newLabel = new QLabel(this);
    newLabel->setFixedWidth(20);
    int fieldIndex = dynamicLayoutVectorB->count() + 1;
    newLabel->setText(QString("%1:").arg(fieldIndex));

    rowLayout->addWidget(newLabel);
    rowLayout->addWidget(newSpinBox);
    dynamicLayoutVectorB->addLayout(rowLayout);
}
//odczyt wszystkich pól wektora A
std::vector<double> ParametryARXWindow::readAllFieldsVectorA()
{
    std::vector<double> values;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(ui->verticalFrame->layout());

    if (mainLayout) {
        for (int i = 0; i < mainLayout->count(); ++i) {
            // Pobieramy układ poziomy (wiersz)
            QLayout *rowLayout = mainLayout->itemAt(i)->layout();
            if (rowLayout) {
                // Szukamy spinboxa w tym układzie
                for (int j = 0; j < rowLayout->count(); ++j) {
                    QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox *>(
                        rowLayout->itemAt(j)->widget());
                    if (sb) {
                        values.push_back(sb->value());
                    }
                }
            }
        }
    }
    // qDebug() << "Pobrano wartości wektor A:" << values;
    return values;
}
//odczyt wszystkich pól wektora B
std::vector<double> ParametryARXWindow::readAllFieldsVectorB()
{
    std::vector<double> values;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(ui->verticalFrame_2->layout());

    if (mainLayout) {
        for (int i = 0; i < mainLayout->count(); ++i) {
            // Pobieramy układ poziomy (wiersz)
            QLayout *rowLayout = mainLayout->itemAt(i)->layout();
            if (rowLayout) {
                // Szukamy spinboxa w tym układzie
                for (int j = 0; j < rowLayout->count(); ++j) {
                    QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox *>(
                        rowLayout->itemAt(j)->widget());
                    if (sb) {
                        values.push_back(sb->value());
                    }
                }
            }
        }
    }
    //qDebug() << "Pobrano wartości wektor B:" << values;
    return values;
}

void ParametryARXWindow::on_addAreaVectorA_clicked()
{
    addNewFieldVectorA();
    addNewFieldVectorB();
}

double ParametryARXWindow::readOpoznienie()
{
    //   qDebug() <<"opoznienie: "<<ui->opoznienie->value();
    return ui->opoznienie->value();
}
double ParametryARXWindow::readSzum()
{
    //qDebug() <<"szum: "<<ui->opoznienie->value();
    return ui->szum->value();
}
Zakres ParametryARXWindow::readZakresSterowania()
{
    Zakres zakresSterowania;
    zakresSterowania.from = ui->odWartoscSterowania->value();
    zakresSterowania.to = ui->doWartoscSterowania->value();
    // qDebug() <<"zakres sterowania:"<<zakresSterowania.from <<"  "<<zakresSterowania.to;
    return zakresSterowania;
}

Zakres ParametryARXWindow::readZakresRegulowania()
{
    Zakres zakresRegulowania;
    zakresRegulowania.from = ui->odWartoscRegulowania->value();
    zakresRegulowania.to = ui->doWartoscRegulowania->value();
    // qDebug() <<"zakres sterowania:"<<zakresSterowania.from <<"  "<<zakresSterowania.to;
    return zakresRegulowania;
}

bool ParametryARXWindow::readCzyOpoznienie()
{
    return ui->checkboxOgraniczenia->isChecked();
}

double ParametryARXWindow::readRegMax()
{
    return ui->doWartoscRegulowania->value();
}
double ParametryARXWindow::readRegMin()
{
    return ui->odWartoscRegulowania->value();
}

double ParametryARXWindow::readMax()
{
    return ui->doWartoscSterowania->value();
}

double ParametryARXWindow::readMin()
{
    return ui->odWartoscSterowania->value();
}

void ParametryARXWindow::refreshFromService()
{
    const QSignalBlocker bK(ui->opoznienie);
    const QSignalBlocker bZ(ui->szum);
    const QSignalBlocker bUmin(ui->odWartoscSterowania);
    const QSignalBlocker bUmax(ui->doWartoscSterowania);
    const QSignalBlocker bYmin(ui->odWartoscRegulowania);
    const QSignalBlocker bYmax(ui->doWartoscRegulowania);

    ui->opoznienie->setValue(uslugi.arx.k.get());
    ui->szum->setValue(uslugi.arx.z.get());

    ui->odWartoscSterowania->setValue(uslugi.arx.limityZadana.getMin());
    ui->doWartoscSterowania->setValue(uslugi.arx.limityZadana.getMax());
    ui->odWartoscRegulowania->setValue(uslugi.arx.limityRegulowana.getMin());
    ui->doWartoscRegulowania->setValue(uslugi.arx.limityRegulowana.getMax());

    // CZYSZCZENIE I ODBUDOWA DYNAMICZNYCH PÓL WEKTORÓW (A i B)
    QLayoutItem *item;
    while ((item = ui->verticalFrame->layout()->takeAt(0)) != nullptr)
    {
        if (item->widget()) delete item->widget();
        delete item;
    }
    while ((item = ui->verticalFrame_2->layout()->takeAt(0)) != nullptr)
    {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // Dodanie nowych pól na podstawie współczynników z serwisu
    for (const auto& w : uslugi.arx.wspolczynniki.value)
    {
        addNewFieldVectorA(w.A);
        addNewFieldVectorB(w.B);
    }
}

ParametryARXWindow::~ParametryARXWindow()
{
    delete ui;
}

void ParametryARXWindow::on_buttonBox_accepted()
{
    readAllFieldsVectorA();
    readAllFieldsVectorB();
    readOpoznienie();
    readSzum();
    readZakresSterowania();

    std::vector<ARX::Wspolczynnik> vec;
    for(auto i : readAllFieldsVectorA())
    {
        vec.push_back(ARX::Wspolczynnik{i, 0.0});
    }
    std::vector<double> vecB = readAllFieldsVectorB();
    for (int var = 0; var < vecB.size(); ++var)
    {
        vec[var].B = vecB[var];
    }

    uslugi.arxChange().instancja->wspolczynniki.value = vec;
    uslugi.arxChange().instancja->k.set(readOpoznienie());
    uslugi.arxChange().instancja->limityZadana.setMax(readMax());
    uslugi.arxChange().instancja->limityZadana.setMin(readMin());
    uslugi.arxChange().instancja->z.set(readSzum());
    uslugi.arxChange().instancja->limityZadana.setActive(readCzyOpoznienie());
    uslugi.arxChange().instancja->limityZadana.setMax(ui->doWartoscRegulowania->value());
    uslugi.arxChange().instancja->limityZadana.setMin(ui->odWartoscRegulowania->value());
    uslugi.arxChange().instancja->limityZadana.setActive(ui->checkboxOgraniczenia->isChecked());

    if(uslugi.trybDzialania.get() != WarstaUslug::TrybDzialania::LOCAL)
        uslugi.netService.sendArxConfig();

    emit uslugi.updateUI();
}
