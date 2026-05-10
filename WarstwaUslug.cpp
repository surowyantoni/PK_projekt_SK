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
    , uar(UAR(&arx, &generator, &pid, &onOff, (UAR::RodzajSterowania)CONSTS::UAR::regulator))
    , timer(this)
    , czas{0}
    , elapsed()
{
    timer.setTimerType(Qt::PreciseTimer); // żeby działał dokładniej
    dziala.set(dziala.get()); // wywoalanie settera - ustawienie timera
    interwal.set(interwal.get()); // wywoalanie settera - ustawienie timera
    QObject::connect(&timer, &QTimer::timeout, this, &WarstaUslug::symuluj);
    elapsed.start();
    // QObject::connect(netService, &NetService::sampleReceivedFromServer, this, &WarstaUslug::sampleReceivedFromREgulatorInstanceNowINeedToForewardItToTheUARAndThenSimmulateARXReactionAndSensTheSignalBack);
    // QObject::connect(netService, &NetService::sampleReceivedFromClient, this, &WarstaUslug::sampleReceivedFromARXObjectNowIHaveToBuildTheTickAndSendItToPlotsToUpdateThem);
}
int WarstaUslug::getBufferFillPercentage()
{
    if(trybDzialania.get() == TrybDzialania::LOCAL)
        return 0;

    return (double)ticki_do_uzupelnienia.size() / CONSTS::NET::MAX_SAMPLES_LAG * 100;
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
    measuredInterval.value = elapsed.elapsed();
    elapsed.start();
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
        timer.stop();
        czas -= interwal.get();
        // throw "WATAFQ"; // Nacisnales przycisk zmiany trybu na ARX, przy włączonej symulacji xD
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
    tick.wartoscRegulowana = sample.wartoscRegulowana;
    uar.zaktualizujPoprzendieWyjscie(sample.wartoscRegulowana);
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
