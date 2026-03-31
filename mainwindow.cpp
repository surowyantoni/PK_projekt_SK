#include "mainwindow.h"
#include "ui_mainwindow.h"
//potrzebne by mainwindow wiedział
#include "parametryarx.h"
#include "connectionwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // --- Regulatory  ---
    //spinboxy
    connect(ui->wzmocnienieP,
            &QDoubleSpinBox::editingFinished,
            this,
            &MainWindow::on_ZapiszRegulatory_clicked);
    connect(ui->stalaCalkowa,
            &QDoubleSpinBox::editingFinished,
            this,
            &MainWindow::on_ZapiszRegulatory_clicked);
    connect(ui->stalaRozniczkowa,
            &QDoubleSpinBox::editingFinished,
            this,
            &MainWindow::on_ZapiszRegulatory_clicked);
    connect(ui->szerokoscHisterezy,
            &QDoubleSpinBox::editingFinished,
            this,
            &MainWindow::on_ZapiszRegulatory_clicked);
    connect(ui->wartoscSterowaniaON,
            &QDoubleSpinBox::editingFinished,
            this,
            &MainWindow::on_ZapiszRegulatory_clicked);
    //radioButtony
    connect(ui->checkBoxPID, &QRadioButton::clicked, this, &MainWindow::on_ZapiszRegulatory_clicked);
    connect(ui->checkBoxOnOff,
            &QRadioButton::clicked,
            this,
            &MainWindow::on_ZapiszRegulatory_clicked);
    connect(ui->pidWewn, &QRadioButton::clicked, this, &MainWindow::on_ZapiszRegulatory_clicked);
    connect(ui->pidZewn, &QRadioButton::clicked, this, &MainWindow::on_ZapiszRegulatory_clicked);
    // --- Generator ---
    //spinboxy
    connect(ui->amplituda, &QSpinBox::editingFinished, this, &MainWindow::on_ustawGenerator_clicked);
    connect(ui->skladowaStala,
            &QSpinBox::editingFinished,
            this,
            &MainWindow::on_ustawGenerator_clicked);
    connect(ui->czasTR, &QSpinBox::editingFinished, this, &MainWindow::on_ustawGenerator_clicked);
    connect(ui->wypelnienie,
            &QDoubleSpinBox::editingFinished,
            this,
            &MainWindow::on_ustawGenerator_clicked);
    //radioButtony
    connect(ui->genWprostokat, &QRadioButton::clicked, this, &MainWindow::on_ustawGenerator_clicked);
    connect(ui->genWsinus, &QRadioButton::clicked, this, &MainWindow::on_ustawGenerator_clicked);

    // ---Wykresy ---
    //spinboxy
    connect(ui->rozmiarWykresu,
            &QSpinBox::editingFinished,
            this,
            &MainWindow::on_ustawWykres_clicked);
    connect(ui->interwal, &QSpinBox::editingFinished, this, &MainWindow::on_ustawWykres_clicked);

    // --- Filtr ---
    //spinboxy
    connect(ui->nasycenieMax,
            &QDoubleSpinBox::editingFinished,
            this,
            &MainWindow::on_ustawFiltr_clicked);
    connect(ui->nasycenieMin,
            &QDoubleSpinBox::editingFinished,
            this,
            &MainWindow::on_ustawFiltr_clicked);
    //radioButtony
    connect(ui->radioWindup, &QRadioButton::clicked, this, &MainWindow::on_ustawFiltr_clicked);
    connect(ui->radioOgraniczenie, &QRadioButton::clicked, this, &MainWindow::on_ustawFiltr_clicked);
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    QButtonGroup *grupaCalka = new QButtonGroup(this);
    grupaCalka->addButton(ui->pidWewn);
    grupaCalka->addButton(ui->pidZewn);

    QButtonGroup *grupaRegulator = new QButtonGroup(this);
    grupaRegulator->addButton(ui->checkBoxPID);
    grupaRegulator->addButton(ui->checkBoxOnOff);

    QButtonGroup *grupaGenerator = new QButtonGroup(this);
    grupaGenerator->addButton(ui->genWprostokat);
    grupaGenerator->addButton(ui->genWsinus);

    QLineSeries *series = new QLineSeries();

    series->append(0, 3);
    series->append(1, 5);
    series->append(2, 7);
    series->append(3, 8);

    graph1 = ui->plot1->addGraph();
    graph1->setPen(QPen(Qt::red));
    graph1->setName("Wartość zadana");
    graph2 = ui->plot1->addGraph();
    graph2->setName("Wartość regulowana");
    ui->plot1->xAxis->setLabel("Czas");
    ui->plot1->xAxis->setRange(0, 10);
    ui->plot1->yAxis->setLabel("Warość zadana i regulowana");
    ui->plot1->yAxis->setRange(-1, 1);
    ui->plot1->legend->setVisible(1);
    //ui->plot1->axisRect()->insetLayout()->remove(ui->plot1->legend);
    //ui->plot1->plotLayout()->addElement(0, 1, ui->plot1->legend);
    //ui->plot1->plotLayout()->setColumnStretchFactor(1, 0.001);
    //ui->plot1->legend->setLayer("legend");

    ui->plot2->addGraph();
    ui->plot2->graph(0)->setName("Uchyb");
    ui->plot2->xAxis->setLabel("Czas");
    ui->plot2->xAxis->setRange(0, 10);
    ui->plot2->yAxis->setLabel("Uchyb");
    ui->plot2->yAxis->setRange(-1, 1);
    ui->plot2->legend->setVisible(1);

    ui->plot3->addGraph();
    ui->plot3->graph(0)->setName("Sygnał sterujący");
    ui->plot3->xAxis->setLabel("Czas");
    ui->plot3->xAxis->setRange(0, 10);
    ui->plot3->yAxis->setLabel("sterowanie");
    ui->plot3->yAxis->setRange(-1, 1);
    ui->plot3->legend->setVisible(1);

    graph3 = ui->plot4->addGraph();
    graph3->setName("Składowa całkowania");
    graph3->setPen(QPen(Qt::red));
    graph4 = ui->plot4->addGraph();
    graph4->setName("Składowa Różniczkowania");
    graph4->setPen(QPen(Qt::blue));
    graph5 = ui->plot4->addGraph();
    graph5->setName("Składowa Proporcjonalna");
    graph5->setPen(QPen(Qt::green));
    ui->plot4->xAxis->setLabel("Czas");
    ui->plot4->xAxis->setRange(0, 10);
    ui->plot4->yAxis->setLabel("skladowe sterowania");
    ui->plot4->yAxis->setRange(-1, 1);
    ui->plot4->legend->setVisible(1);

    time = 0.0;

    m_uslugi.setOnDataReady(
        [this]() { QMetaObject::invokeMethod(this, "updateWykres", Qt::QueuedConnection); });

    wczytajWartosciZeSciezki(":/wzor/wzor.json");

    //m_uslugi.symulacja();
    //updateWykres();
    graph1->addData(0, 0);
    graph2->addData(0, 0);
    ui->plot2->graph()->addData(0, 0);
    ui->plot3->graph()->addData(0, 0);
    graph3->addData(0, 0);
    graph4->addData(0, 0);
    graph5->addData(0, 0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateWykres()
{
    double wartoscRegulowana = m_uslugi.getWartoscR();
    wartoscZ = m_uslugi.getWartoscZadana();
    double uchyb = m_uslugi.getUchyb();
    double wartoscSterowania = m_uslugi.getWartoscSterowania();
    double skladowaC = m_uslugi.getSkladowaC();
    double skladowaR = m_uslugi.getSkladowaR();
    double skladowaP = m_uslugi.getSkladowaP();
    time = m_uslugi.getCzas();
    //qDebug() << "time " << time;

    graph1->addData(time, wartoscZ);
    graph2->addData(time, wartoscRegulowana);
    if (time > range)
        ui->plot1->xAxis->setRange(time - range, time);
    if (wartoscZ - up1 >= 0 || wartoscRegulowana - up1 >= 0) {
        if (wartoscZ > wartoscRegulowana) {
            //ui->plot1->yAxis->setRangeUpper(1+wartoscZ);
            up1 = wartoscZ + (wartoscZ * 0.1);
        } else {
            up1 = wartoscRegulowana + (wartoscRegulowana * 0.1);
        }
    }
    if (wartoscZ - lp1 < 0 || wartoscRegulowana - lp1 < 0) {
        if (wartoscZ < wartoscRegulowana) {
            //ui->plot1->yAxis->setRangeLower(wartoscZ-1);
            lp1 = wartoscZ + (wartoscZ * 0.1);
        } else {
            //ui->plot1->yAxis->setRangeLower(wartoscRegulowana-1);
            lp1 = wartoscRegulowana + (wartoscRegulowana * 0.1);
        }
    }
    ui->plot1->yAxis->setRange(lp1, up1);
    ui->plot1->replot();

    ui->plot2->graph(0)->addData(time, uchyb);
    if (time > range)
        ui->plot2->xAxis->setRange(time - range, time);
    if (uchyb - up2 > 0) {
        //ui->plot1->yAxis->setRangeUpper(1+uchyb);
        up2 = uchyb + (uchyb * 0.1);
    }
    if (uchyb - lp2 < 0) {
        //ui->plot1->yAxis->setRangeLower(uchyb-1);
        lp2 = uchyb + (uchyb * 0.1);
    }
    ui->plot2->yAxis->setRange(lp2, up2);
    ui->plot2->replot();

    ui->plot3->graph(0)->addData(time, wartoscSterowania);
    if (time > range)
        ui->plot3->xAxis->setRange(time - range, time);
    if (wartoscSterowania - up3 > 0) {
        //ui->plot1->yAxis->setRangeUpper(1+wartoscSterowania);
        up3 = wartoscSterowania + (wartoscSterowania * 0.1);
    }
    if (wartoscSterowania - lp3 < 0) {
        //ui->plot1->yAxis->setRangeLower(wartoscSterowania-1);
        lp3 = wartoscSterowania + (wartoscSterowania * 0.1);
    }
    ui->plot3->yAxis->setRange(lp3, up3);
    ui->plot3->replot();

    graph3->addData(time, skladowaC);
    graph4->addData(time, skladowaR);
    graph5->addData(time, skladowaP);
    if (time > range)
        ui->plot4->xAxis->setRange(time - range, time);
    if (skladowaC - up4 > 0 || skladowaR - up4 > 0 || skladowaP - up4 > 0) {
        if (skladowaC > skladowaR && skladowaC > skladowaP) {
            //ui->plot1->yAxis->setRangeUpper(1+skladowaC);
            up4 = skladowaC + (skladowaC * 0.1);
        } else if (skladowaR > skladowaC && skladowaR > skladowaP) {
            //ui->plot1->yAxis->setRangeUpper(1+skladowaR);
            up4 = skladowaR + (skladowaR * 0.1);
        } else {
            //ui->plot1->yAxis->setRangeUpper(1+skladowaP);

            up4 = skladowaP + (skladowaP * 0.1);
        }
    }
    if (skladowaC - lp4 < 0 || skladowaR - lp4 < 0 || skladowaP - lp4 < 0) {
        if (skladowaC < skladowaR && skladowaC < skladowaP) {
            //ui->plot1->yAxis->setRangeUpper(skladowaC-1);
            lp4 = skladowaC + (skladowaC * 0.1);
        } else if (skladowaR < skladowaC && skladowaR < skladowaP) {
            //ui->plot1->yAxis->setRangeUpper(skladowaR-1);
            lp4 = skladowaR + (skladowaR * 0.1);
        } else {
            //ui->plot1->yAxis->setRangeUpper(skladowaP-1);
            lp4 = skladowaP + (skladowaP * 0.1);
        }
    }
    ui->plot4->yAxis->setRange(lp4, up4);
    ui->plot4->replot();

    /*if (time==5){
        qDebug() <<"key 1"<<(graph1->data()->constBegin())->sortKey();
        qDebug() <<"key 2"<<(graph1->data()->constBegin()+2)->sortKey();
        qDebug() <<"key 3"<<(graph1->data()->constBegin()+4)->sortKey();
        qDebug() <<"key 4"<<(graph1->data()->constBegin()+6)->sortKey();
        qDebug() <<"key last"<<(graph1->data()->constEnd()-1)->sortKey();
    }*/
    if (time > 50) {
        graph1->data()->removeBefore(time - 50);
        graph2->data()->removeBefore(time - 50);
        ui->plot2->graph()->data()->removeBefore(time - 50);
        ui->plot3->graph()->data()->removeBefore(time - 50);
        graph3->data()->removeBefore(time - 50);
        graph4->data()->removeBefore(time - 50);
        graph5->data()->removeBefore(time - 50);
    }

    //time+=(freq/1000);
}

double MainWindow::readWzmocnienie()
{
    //qDebug() << "wzmocnienie " << ui->wzmocnienieP->value();
    return ui->wzmocnienieP->value();
}
double MainWindow::readStalaCalkowania()
{
    //qDebug() << "stala c " << ui->stalaCalkowa->value();
    return ui->stalaCalkowa->value();
}
double MainWindow::readStalaRozniczkowania()
{
    // qDebug() << "stala r " << ui->stalaRozniczkowa->value();
    return ui->stalaRozniczkowa->value();
}
double MainWindow::readSzerokoscHisterezy()
{
    // qDebug() << "histereza " << ui->szerokoscHisterezy->value();
    return ui->szerokoscHisterezy->value();
}
double MainWindow::readWartoscSterowaniaOn()
{
    //qDebug() << "ster on " << ui->wartoscSterowaniaON->value();
    return ui->wartoscSterowaniaON->value();
}
RegulatorPID::SposobLiczeniaCalki MainWindow::readSposobLiczeniaPID()
{
    //1-wew 0-zew
    if (ui->pidWewn->isChecked()) {
        return RegulatorPID::Wewnetrzne;
    } else if (ui->pidZewn->isChecked()) {
        return RegulatorPID::Zewnetrzne;
    }
}

int MainWindow::readCzasRzeczywisty()
{
    return ui->czasTR->value();
}
int MainWindow::readAmplituda()
{
    return ui->amplituda->value();
}
int MainWindow::readSkladowaStala()
{
    return ui->skladowaStala->value();
}
double MainWindow::readWypelnienie()
{
    return ui->wypelnienie->value();
}
int MainWindow::readTypGeneratora()
{
    if (ui->genWprostokat->isChecked()) {
        return 0;
    } else if (ui->genWsinus->isChecked()) {
        return 1;
    }
}

bool MainWindow::readPIDCheckBox()
{
    //qDebug() << "pid check " << ui->checkBoxPID->isChecked();
    return ui->checkBoxPID->isChecked();
}
bool MainWindow::readONOFFCheckBox()
{
    //qDebug() << "onoff check " << ui->checkBoxOnOff->isChecked();
    return ui->checkBoxOnOff->isChecked();
}

void MainWindow::on_ZapiszRegulatory_clicked()
{
    if (readPIDCheckBox()) {
        m_uslugi.set_wybranyRegulator(PID);
        m_uslugi.set_m_wzmocnienie(readWzmocnienie());
        m_uslugi.set_m_stalaCalkowania(readStalaCalkowania());
        m_uslugi.set_m_stalaRozniczkowania(readStalaRozniczkowania());
        m_uslugi.set_m_sposobLiczeniaCalki(readSposobLiczeniaPID());

        m_uslugi.set_m_pid();
        m_uslugi.setPID();
    }

    if (readONOFFCheckBox()) {
        m_uslugi.set_wybranyRegulator(ONOFF);
        m_uslugi.set_m_h(readSzerokoscHisterezy());
        m_uslugi.set_m_uON(readWartoscSterowaniaOn());
        m_uslugi.set_m_onOff();
        m_uslugi.setOnOff();
    }
}

void MainWindow::on_StartStop_clicked()
{
    m_uslugi.rozpocznij();
}
void MainWindow::on_ustawWartoscZ_clicked()
{
    // wartoscZ = ui->wartoscZadana->value();
}

void MainWindow::on_ustawWykres_clicked()
{
    range = readWielkoscWykresu();
    if (time > range) {
        ui->plot1->xAxis->setRange(time - range, time);
        ui->plot2->xAxis->setRange(time - range, time);
        ui->plot3->xAxis->setRange(time - range, time);
        ui->plot4->xAxis->setRange(time - range, time);
    } else {
        ui->plot1->xAxis->setRange(0, range);
        ui->plot2->xAxis->setRange(0, range);
        ui->plot3->xAxis->setRange(0, range);
        ui->plot4->xAxis->setRange(0, range);
    }

    ui->plot1->replot();
    ui->plot2->replot();
    ui->plot3->replot();
    ui->plot4->replot();

    m_uslugi.setFreq(ui->interwal->value());
    //qDebug() <<"key 3"<<freq/1000;
    //timer->setInterval(freq);
}

int MainWindow::readWielkoscWykresu()
{
    return ui->rozmiarWykresu->value();
}

void MainWindow::on_InterfejsARX_clicked()
{
    ParametryARX *popup = new ParametryARX(this);
    //blokada okna głównego gdy popup sie wyświetli
    popup->exec();
    // Opcjonalnie: automatyczne usuwanie z pamięci po zamknięciu (sie zobaczy czy bedzei potrzebne)
    //popup->setAttribute(Qt::WA_DeleteOnClose);
}

void MainWindow::on_pushButton_clicked() {}
void MainWindow::on_Reset_clicked()
{
    m_uslugi.resetCzas();
    graph1->data()->clear();
    graph2->data()->clear();
    ui->plot2->graph()->data()->clear();
    ui->plot3->graph()->data()->clear();
    graph3->data()->clear();
    graph4->data()->clear();
    graph5->data()->clear();
    ui->plot1->yAxis->setRange(-1, 1);
    ui->plot1->xAxis->setRange(0, range);
    ui->plot2->yAxis->setRange(-1, 1);
    ui->plot2->xAxis->setRange(0, range);
    ui->plot3->yAxis->setRange(-1, 1);
    ui->plot3->xAxis->setRange(0, range);
    ui->plot4->yAxis->setRange(-1, 1);
    ui->plot4->xAxis->setRange(0, range);
    ui->plot1->replot();
    ui->plot2->replot();
    ui->plot3->replot();
    ui->plot4->replot();
    up1 = 1;
    lp1 = -1;
    up2 = 1;
    lp2 = -1;
    up3 = 1;
    lp3 = -1;
    up4 = 1;
    lp4 = -1;
}

void MainWindow::on_ustawGenerator_clicked()
{
    m_uslugi.set_Amplituda(readAmplituda());
    m_uslugi.set_stalaCzasowa(readSkladowaStala());
    m_uslugi.set_czasRzeczywisty(readCzasRzeczywisty());
    m_uslugi.set_choice(readTypGeneratora());
    m_uslugi.set_wypelnienie(readWypelnienie());
}

void MainWindow::on_actionZapisz_triggered()
{
    QJsonObject UAR;
    QJsonArray wspolczynnikiA;
    QJsonArray wspolczynnikiB;

    for (double wartosc : m_uslugi.get_m_WspolczynnikiA()) {
        wspolczynnikiA.append(wartosc);
    }
    for (double wartosc : m_uslugi.get_m_WspolczynnikiB()) {
        wspolczynnikiB.append(wartosc);
    }

    QJsonObject ARX;
    ARX["opoznienie"] = m_uslugi.get_m_OpoznienieTransportowe();
    ARX["ograniczenieSterowaniaMax"] = m_uslugi.get_m_OgraniczenieMax();
    ARX["ograniczenieSterowaniaMin"] = m_uslugi.get_m_OgraniczenieMin();
    ARX["ograniczenieRegulowaniaMax"] = m_uslugi.get_m_ograniczenieRegulowaniaMax();
    ARX["ograniczenieRegulowaniaMin"] = m_uslugi.get_m_ograniczenieRegulowaniaMin();
    ARX["wspolczynnikiA"] = wspolczynnikiA;
    ARX["wspolczynnikiB"] = wspolczynnikiB;
    ARX["CzyOgraniczac"] = m_uslugi.get_m_czyOgraniczenie();

    QJsonObject Regulator;
    Regulator["wybranyRegulator"] = m_uslugi.get_wybranyRegulator();
    if (m_uslugi.get_wybranyRegulator() == PID) {
        Regulator["WzmocnienieK"] = m_uslugi.get_m_wzmocnienie();
        Regulator["StalaR"] = m_uslugi.get_m_stalaRozniczkowania();
        Regulator["StalaC"] = m_uslugi.get_m_stalaCalkowania();
        Regulator["SposobLiczeniaCalki"] = m_uslugi.get_m_sposobLiczeniaCalki();
    } else if (m_uslugi.get_wybranyRegulator() == ONOFF) {
        Regulator["SzerokoscHisterezy"] = m_uslugi.get_m_h();
        Regulator["WartoscSterowaniaON"] = m_uslugi.get_m_uON();
    }

    QJsonObject GeneratorW;
    GeneratorW["TrybPracy"] = m_uslugi.get_m_c();
    GeneratorW["CzasRzeczywisty"] = m_uslugi.get_m_Tr();
    GeneratorW["Amplituda"] = m_uslugi.get_m_A();
    GeneratorW["SkladowaStala"] = m_uslugi.get_m_S();
    GeneratorW["Wypelnienie"] = m_uslugi.get_m_p();

    QJsonObject Filtr;
    Filtr["od"] = ui->nasycenieMin->value();
    Filtr["do"] = ui->nasycenieMax->value();

    QJsonObject Wykresy;
    Wykresy["WielkoscWykresu"] = ui->rozmiarWykresu->value();
    Wykresy["Interwal"] = ui->interwal->value();

    if (ui->radioWindup->isChecked()) { //0-antiwindup, 1-ograniczenie
        Filtr["ktoryFiltr"] = 0;
    } else if (ui->radioOgraniczenie->isChecked()) {
        Filtr["ktoryFiltr"] = 1;
    }

    UAR["ARX"] = ARX;
    UAR["Regulator"] = Regulator;
    UAR["GeneratorW"] = GeneratorW;
    UAR["Filtr"] = Filtr;
    UAR["Wykresy"] = Wykresy;

    QString sciezka = QFileDialog::getSaveFileName(nullptr,
                                                   "Zapisz dane jako JSON",
                                                   QDir::homePath(),
                                                   "Pliki JSON (*.json);;Tekst (*.txt)");

    if (!sciezka.isEmpty()) {
        QFile plik(sciezka);
        if (plik.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(UAR);
            plik.write(doc.toJson());
            plik.close();
        }
    }
}

void MainWindow::on_actionOtworz_triggered()
{
    QString sciezka = QFileDialog::getOpenFileName(this,
                                                   "Wczytaj dane z JSON",
                                                   QDir::homePath(),
                                                   "Pliki JSON (*.json);;Wszystkie pliki (*)");

    if (sciezka.isEmpty())
        return;

    // Wywołanie naszej nowej funkcji
    wczytajWartosciZeSciezki(sciezka);

    QMessageBox::information(this, "Sukces", "Dane zostały wczytane.");
}

void MainWindow::on_actionPolacz_triggered()
{
    if (!connWindow)
    {
        service = new NetService(this);
        connWindow = new ConnectionWindow(service);
        connWindow->setAttribute(Qt::WA_DeleteOnClose);

        connect(connWindow, &QObject::destroyed, this, [this]()
                {
                    connWindow = nullptr;
                });
        connect(service, &NetService::pidUpdated, this, &MainWindow::onPidUpdated);
    }


    connWindow->show();
    connWindow->setWindowState(Qt::WindowNoState);
    connWindow->raise();
    connWindow->activateWindow();
}

void MainWindow::readNasycenieMax()
{
    m_uslugi.set_m_nasycenieMax(ui->nasycenieMax->value());
}

void MainWindow::readNasycenieMin()
{
    m_uslugi.set_m_nasycenieMin(ui->nasycenieMin->value());
}

void MainWindow::czyWindup()
{
    m_uslugi.set_m_windup(ui->radioWindup->isChecked());
}
void MainWindow::czyNasycenie()
{
    m_uslugi.set_m_nasycenie(ui->radioOgraniczenie->isChecked());
}

void MainWindow::on_ustawFiltr_clicked()
{
    if (ui->radioOgraniczenie->isChecked()) {
        readNasycenieMax();
        readNasycenieMin();
    }
    czyNasycenie();
    czyWindup();
}

void MainWindow::on_resetPID_clicked()
{
    m_uslugi.resetCalkowania();
}

void MainWindow::wczytajWartosciZeSciezki(QString sciezka)
{
    QFile plik(sciezka);
    if (!plik.open(QIODevice::ReadOnly)) {
        qDebug() << "Nie można otworzyć pliku: " << sciezka;
        return;
    }

    QByteArray dane = plik.readAll();
    plik.close();

    QJsonDocument doc = QJsonDocument::fromJson(dane);
    if (doc.isNull()) {
        qDebug() << "Błąd formatu JSON w pliku: " << sciezka;
        return;
    }

    QJsonObject UAR = doc.object();

    // --- ARX ---
    if (UAR.contains("ARX")) {
        QJsonObject ARX = UAR["ARX"].toObject();

        QJsonArray jsonWspA = ARX["wspolczynnikiA"].toArray();
        std::vector<double> wektorA;
        for (const auto &val : jsonWspA)
            wektorA.push_back(val.toDouble());

        QJsonArray jsonWspB = ARX["wspolczynnikiB"].toArray();
        std::vector<double> wektorB;
        for (const auto &val : jsonWspB)
            wektorB.push_back(val.toDouble());

        m_uslugi.set_wspolczynniki(wektorA, wektorB);

        m_uslugi.set_m_opoznienieTransportowe(ARX["opoznienie"].toDouble());
        m_uslugi.set_m_ograniczenieMaxZad(ARX["ograniczenieSterowaniaMax"].toDouble());
        m_uslugi.set_m_ograniczenieMinZad(ARX["ograniczenieSterowaniaMin"].toDouble());
        m_uslugi.set_m_ograniczenieMaxReg(ARX["ograniczenieRegulowaniaMax"].toDouble());
        m_uslugi.set_m_ograniczenieMinReg(ARX["ograniczenieRegulowaniaMin"].toDouble());
        m_uslugi.set_m_czyOgraniczenie(ARX["CzyOgraniczac"].toBool());

        m_uslugi.set_m_arx();
    }

    // --- REGULATOR ---
    if (UAR.contains("Regulator")) {
        QJsonObject Regulator = UAR["Regulator"].toObject();

        int regType = Regulator["wybranyRegulator"].toInt();
        WybranyRegulator regT;

        ui->checkBoxPID->setChecked(false);
        ui->checkBoxOnOff->setChecked(false);

        switch (regType) {
        case 0: // PID
            regT = PID;
            ui->checkBoxPID->setChecked(true);
            break;
        case 1: // ONOFF
            regT = ONOFF;
            ui->checkBoxOnOff->setChecked(true);
            break;
        default:
            regT = nic;
            break;
        }

        m_uslugi.set_wybranyRegulator(regT);

        if (regT == PID) {
            double k = Regulator["WzmocnienieK"].toDouble();
            double r = Regulator["StalaR"].toDouble();
            double c = Regulator["StalaC"].toDouble();

            m_uslugi.set_m_wzmocnienie(k);
            m_uslugi.set_m_stalaRozniczkowania(r);
            m_uslugi.set_m_stalaCalkowania(c);

            ui->wzmocnienieP->setValue(k);
            ui->stalaRozniczkowa->setValue(r);
            ui->stalaCalkowa->setValue(c);

            int calkaMode = Regulator["SposobLiczeniaCalki"].toInt();
            if (calkaMode == 0) {
                m_uslugi.set_m_sposobLiczeniaCalki(RegulatorPID::Wewnetrzne);
                ui->pidWewn->setChecked(true);
            } else {
                m_uslugi.set_m_sposobLiczeniaCalki(RegulatorPID::Zewnetrzne);
                ui->pidZewn->setChecked(true);
            }
        }

        if (regT == ONOFF) {
            double hist = Regulator["SzerokoscHisterezy"].toDouble();
            double sterOn = Regulator["WartoscSterowaniaON"].toDouble();

            m_uslugi.set_m_h(hist);
            m_uslugi.set_m_uON(sterOn);

            m_uslugi.set_m_wzmocnienie(0);
            m_uslugi.set_m_stalaRozniczkowania(0);
            m_uslugi.set_m_stalaCalkowania(0);
            ui->wzmocnienieP->setValue(0);
            ui->stalaRozniczkowa->setValue(0);
            ui->stalaCalkowa->setValue(0);

            // Hack na odświeżenie radiobuttonów
            ui->pidWewn->setAutoExclusive(false);
            ui->pidZewn->setAutoExclusive(false);
            ui->pidWewn->setChecked(false);
            ui->pidZewn->setChecked(false);
            ui->pidWewn->setAutoExclusive(true);
            ui->pidZewn->setAutoExclusive(true);

            ui->szerokoscHisterezy->setValue(hist);
            ui->wartoscSterowaniaON->setValue(sterOn);
        }
    }

    // --- GENERATOR W ---
    if (UAR.contains("GeneratorW")) {
        QJsonObject GeneratorW = UAR["GeneratorW"].toObject();

        int tryb = GeneratorW["TrybPracy"].toInt();
        double czasRz = GeneratorW["CzasRzeczywisty"].toDouble();
        double ampl = GeneratorW["Amplituda"].toDouble();
        double skla = GeneratorW["SkladowaStala"].toDouble();
        double wyp = GeneratorW["Wypelnienie"].toDouble();

        m_uslugi.set_choice(tryb);
        m_uslugi.set_czasRzeczywisty(czasRz);
        m_uslugi.set_Amplituda(ampl);
        m_uslugi.set_stalaCzasowa(skla);
        m_uslugi.set_wypelnienie(wyp);

        ui->czasTR->setValue(czasRz);
        ui->amplituda->setValue(ampl);
        ui->skladowaStala->setValue(skla);
        ui->wypelnienie->setValue(wyp);

        if (tryb == 1)
            ui->genWprostokat->setChecked(true);
        else
            ui->genWsinus->setChecked(true);
    }

    // --- FILTR ---
    if (UAR.contains("Filtr")) {
        QJsonObject Filtr = UAR["Filtr"].toObject();
        m_uslugi.set_m_nasycenieMax(Filtr["do"].toDouble());
        m_uslugi.set_m_nasycenieMin(Filtr["od"].toDouble());
        ui->nasycenieMin->setValue(Filtr["od"].toDouble());
        ui->nasycenieMax->setValue(Filtr["do"].toDouble());

        if (Filtr["ktoryFiltr"].toInt() == 0) {
            m_uslugi.set_m_windup(1);
            ui->radioWindup->setChecked(true);
        } else {
            m_uslugi.set_m_nasycenie(1);
            ui->radioOgraniczenie->setChecked(true);
        }
    }

    if (UAR.contains("Wykresy")) {
        QJsonObject Wykresy = UAR["Wykresy"].toObject();
        qDebug() << Wykresy["WielkoscWykresu"].toInt();
        qDebug() << Wykresy["Interwal"].toInt();
        ui->rozmiarWykresu->setValue(Wykresy["WielkoscWykresu"].toInt());
        ui->interwal->setValue(Wykresy["Interwal"].toInt());
    }

    on_ZapiszRegulatory_clicked();
    on_ustawGenerator_clicked();
    on_ustawWykres_clicked();
}

void MainWindow::applyNetworkRole(bool isRegulator)
{
    m_isRegulator = isRegulator;

    if (isRegulator) { // ROLA: REGULATOR (KLIENT)
        // ui->groupARX->setEnabled(false);    // Regulator nie zmienia modelu obiektu [14]
        // ui->groupPID->setEnabled(true);
        // ui->groupGenerator->setEnabled(true);
        // ui->groupSimParams->setEnabled(true); // Zarządza Start/Stop/Reset [1]
        // log("Tryb Sieciowy: REGULATOR. Blokada edycji ARX.");
    } else { // ROLA: OBIEKT (SERWER)
        // ui->groupARX->setEnabled(true);
        // ui->groupPID->setEnabled(false);    // Obiekt nie steruje [14]
        // ui->groupGenerator->setEnabled(false);
        // ui->groupSimParams->setEnabled(false); // Czeka na polecenia z sieci
        // log("Tryb Sieciowy: OBIEKT. Blokada sterowania i generatora.");
    }
}
void MainWindow::onPidUpdated(double p, double i, double d, int mode, double min, double max)
{
    m_uslugi.set_m_wzmocnienie(p);
    m_uslugi.set_m_stalaCalkowania(i);
    m_uslugi.set_m_stalaRozniczkowania(d);

    if(mode)
        ui->pidWewn->click();
    else
        ui->pidZewn->click();
    ui->wzmocnienieP->setValue(p);
    ui->stalaCalkowa->setValue(i);
    ui->stalaRozniczkowa->setValue(d);
    ui->nasycenieMin->setValue(min);
    ui->nasycenieMax->setValue(max);

}
void MainWindow::onGenUpdated(int type, double amp, double per, double off, double duty)
{

}
void MainWindow::onSampleReceived(double u, double y, quint32 k)
{

}
void MainWindow::onArxUpdated(const QVector<double>& A, const QVector<double>& B, int k, double sigma, double minU, double maxU, double minY, double maxY)
{

}
