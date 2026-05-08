#include "WarstwaUslug.h"




WarstaUslug::WarstaUslug()
    : trybDzialania{TrybDzialania::LOCAL, this}
    , interwal{CONSTS::UAR::interwal, this}
    , dziala{CONSTS::UAR::started, this}
    , regulacja{(UAR::RodzajSterowania)CONSTS::UAR::regulator, this}
    , netService(NetService(this))
    , arx(ARX())
    , pid(RegulatorPID())
    , onOff(RegulatorOnOff())
    , generator(GeneratorWartosci())
    , uar(UAR(&arx, &generator, &pid))
    , timer(this)
    , czas{0}
{
    timer.setTimerType(Qt::PreciseTimer); // żeby działał dokładniej
    dziala.set(dziala.get()); // wywoalanie settera - ustawienie timera
    interwal.set(interwal.get()); // wywoalanie settera - ustawienie timera
    QObject::connect(&timer, &QTimer::timeout, this, &WarstaUslug::symuluj);
    // QObject::connect(netService, &NetService::sampleReceivedFromServer, this, &WarstaUslug::sampleReceivedFromREgulatorInstanceNowINeedToForewardItToTheUARAndThenSimmulateARXReactionAndSensTheSignalBack);
    // QObject::connect(netService, &NetService::sampleReceivedFromClient, this, &WarstaUslug::sampleReceivedFromARXObjectNowIHaveToBuildTheTickAndSendItToPlotsToUpdateThem);
}
int WarstaUslug::getBufferFillPercentage()
{
    if(trybDzialania.get() == TrybDzialania::LOCAL)
        return 0;

    return (double)ticki_do_uzupelnienia.size() / CONSTS::NET::MAX_SAMPLES_LAG;
}
void WarstaUslug::reset()
{
    arx.reset();
    pid.reset();
    onOff.reset();
    generator.reset();
    czas = 0;
}

void WarstaUslug::symuluj()
{
    czas = interwal.get() + czas;
    switch (trybDzialania.get())
    {
    case TrybDzialania::LOCAL:
        emit updateCharts(uar.symuluj(interwal.get()), czas);
        break;
    case TrybDzialania::NET_REG:
        ticki_do_uzupelnienia.push(uar.symulujBezObiektu(interwal.get()));
        netService.sendSample(SimSampleFromRegulator {
          .wartoscZadana = ticki_do_uzupelnienia.back().wartoscZadana,
          .sterowanie = ticki_do_uzupelnienia.back().sterowanie
        });
        break;
    case TrybDzialania::NET_ARX:
        throw "WATAFQ";
        break;
    }
}

void WarstaUslug::sampleReceivedFromREgulatorInstanceNowINeedToForewardItToTheUARAndThenSimmulateARXReactionAndSensTheSignalBack(SimSampleFromRegulator sample)
{
    czas = interwal.get() + czas;
    double wartoscRegulowana = uar.symulujObiekt(sample.sterowanie);
    UAR::Tick tick;
    tick.wartoscRegulowana = wartoscRegulowana;
    tick.wartoscZadana = sample.wartoscZadana;
    tick.sterowanie = sample.sterowanie;
    tick.uchyb = 0.0; // TYMCZASOWO
    tick.pid = std::nullopt;
    emit updateCharts(tick, czas);

    SimSampleFromObject newSample {wartoscRegulowana};
    netService.sendSample(newSample);
}

void WarstaUslug::sampleReceivedFromARXObjectNowIHaveToBuildTheTickAndSendItToPlotsToUpdateThem(SimSampleFromObject sample)
{
    UAR::Tick tick = ticki_do_uzupelnienia.front();
    ticki_do_uzupelnienia.pop();
    if(ticki_do_uzupelnienia.size() > 1)
        qDebug() << "SYMULACJA SIE NIE WYRABIA!!!!";
    tick.wartoscRegulowana = sample.wartoscRegulowana;
    uar.zaktualizujPoprzendieWyjscie(sample.wartoscRegulowana);
    qDebug() << "DZIAŁA ";
    emit updateCharts(tick, czas);
}


void WarstaUslug::wczytajZPliku()
{
    //TODO
    emit updateUI();
}
void WarstaUslug::zapiszDoPliku()
{
    //TODO
}
