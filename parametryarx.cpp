#include "parametryarx.h"
#include "mainwindow.h"
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

    std::vector<double> vecA = m_parent->m_uslugi.get_m_WspolczynnikiA();
    std::vector<double> vecB = m_parent->m_uslugi.get_m_WspolczynnikiB();

    if (vecA.empty()) {
        for (int i = 0; i < 3; ++i)
            addNewFieldVectorA(0.0);
    } else {
        for (double val : vecA) {
            addNewFieldVectorA(val);
        }
    }

    if (vecB.empty()) {
        for (int i = 0; i < 3; ++i)
            addNewFieldVectorB(0.0);
    } else {
        for (double val : vecB) {
            addNewFieldVectorB(val);
        }
    }

    ui->opoznienie->setValue(m_parent->m_uslugi.get_m_OpoznienieTransportowe());

    ui->szum->setValue(m_parent->m_uslugi.get_m_Zaklucenie());
    ui->checkboxOgraniczenia->setChecked(m_parent->m_uslugi.get_m_czyOgraniczenie());

    ui->odWartoscSterowania->setValue(m_parent->m_uslugi.get_m_OgraniczenieMin());
    ui->doWartoscSterowania->setValue(m_parent->m_uslugi.get_m_OgraniczenieMax());
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

    m_parent->m_uslugi.set_wspolczynniki(readAllFieldsVectorA(), readAllFieldsVectorB());
    m_parent->m_uslugi.set_m_opoznienieTransportowe(readOpoznienie());
    m_parent->m_uslugi.set_m_ograniczenieMaxZad(readMax());
    m_parent->m_uslugi.set_m_ograniczenieMinZad(readMin());
    m_parent->m_uslugi.set_m_wartoscZaklocen(readSzum());
    m_parent->m_uslugi.set_m_ograniczenieMaxReg(readRegMax());
    m_parent->m_uslugi.set_m_ograniczenieMinReg(readRegMin());
    m_parent->m_uslugi.set_m_arx();
    m_parent->m_uslugi.set_m_czyOgraniczenie(readCzyOpoznienie());

    m_parent->m_uslugi.set_m_ograniczenieMaxReg(ui->doWartoscRegulowania->value());
    m_parent->m_uslugi.set_m_ograniczenieMinReg(ui->odWartoscRegulowania->value());

    m_parent->m_uslugi.set_m_czyOgraniczenie(ui->checkboxOgraniczenia->isChecked());
}
