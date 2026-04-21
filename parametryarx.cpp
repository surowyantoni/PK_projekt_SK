#include "parametryarx.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_parametryarx.h"
#include <vector>

ParametryARX::ParametryARX(MainWindow *parentWindow)
    : QDialog(parentWindow)
    , ui(new Ui::ParametryARX)
{
    m_parent = parentWindow;
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

    std::vector<ARX::Wspolczynnik> vec = m_parent->uslugi.arx.wspolczynniki;

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

    ui->opoznienie->setValue(m_parent->uslugi.arx.k);

    ui->szum->setValue(m_parent->uslugi.arx.z);
    ui->checkboxOgraniczenia->setChecked(m_parent->uslugi.arx.limityZadana.getActive());

    ui->odWartoscSterowania->setValue(m_parent->uslugi.arx.limityZadana.getMin());
    ui->doWartoscSterowania->setValue(m_parent->uslugi.arx.limityZadana.getMax());
}

// Zmodyfikowana funkcja dodająca pole z wartością
void ParametryARX::addNewFieldVectorA(double value)
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
void ParametryARX::addNewFieldVectorB(double value)
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
std::vector<double> ParametryARX::readAllFieldsVectorA()
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
std::vector<double> ParametryARX::readAllFieldsVectorB()
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

void ParametryARX::on_addAreaVectorA_clicked()
{
    addNewFieldVectorA();
    addNewFieldVectorB();
}

double ParametryARX::readOpoznienie()
{
    //   qDebug() <<"opoznienie: "<<ui->opoznienie->value();
    return ui->opoznienie->value();
}
double ParametryARX::readSzum()
{
    //qDebug() <<"szum: "<<ui->opoznienie->value();
    return ui->szum->value();
}
Zakres ParametryARX::readZakresSterowania()
{
    Zakres zakresSterowania;
    zakresSterowania.from = ui->odWartoscSterowania->value();
    zakresSterowania.to = ui->doWartoscSterowania->value();
    // qDebug() <<"zakres sterowania:"<<zakresSterowania.from <<"  "<<zakresSterowania.to;
    return zakresSterowania;
}

Zakres ParametryARX::readZakresRegulowania()
{
    Zakres zakresRegulowania;
    zakresRegulowania.from = ui->odWartoscRegulowania->value();
    zakresRegulowania.to = ui->doWartoscRegulowania->value();
    // qDebug() <<"zakres sterowania:"<<zakresSterowania.from <<"  "<<zakresSterowania.to;
    return zakresRegulowania;
}

bool ParametryARX::readCzyOpoznienie()
{
    return ui->checkboxOgraniczenia->isChecked();
}

double ParametryARX::readRegMax()
{
    return ui->doWartoscRegulowania->value();
}
double ParametryARX::readRegMin()
{
    return ui->odWartoscRegulowania->value();
}

double ParametryARX::readMax()
{
    return ui->doWartoscSterowania->value();
}

double ParametryARX::readMin()
{
    return ui->odWartoscSterowania->value();
}

ParametryARX::~ParametryARX()
{
    delete ui;
}

void ParametryARX::on_buttonBox_accepted()
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

    m_parent->uslugi.arx.wspolczynniki.set(vec);
    m_parent->uslugi.arx.k = (readOpoznienie());
    m_parent->uslugi.arx.limityZadana.setMax(readMax());
    m_parent->uslugi.arx.limityZadana.setMin(readMin());
    m_parent->uslugi.arx.z = readSzum();
    m_parent->uslugi.arx.limityZadana.setActive(readCzyOpoznienie());

    m_parent->uslugi.arx.limityZadana.setMax(ui->doWartoscRegulowania->value());
    m_parent->uslugi.arx.limityZadana.setMin(ui->odWartoscRegulowania->value());

    m_parent->uslugi.arx.limityZadana.setActive(ui->checkboxOgraniczenia->isChecked());

    m_parent->ui->pushButton_arx->setEnabled(true);
}
