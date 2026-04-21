#include "WarstwaUslug.h"




WarstaUslug::WarstaUslug()
    : interwal(200)
    , czas(0)
    , dziala(this, false)
    , arx(ARX())
    , pid(RegulatorPID())
    , onOff(RegulatorOnOff())
    , generator(GeneratorWartosci())
    , uar(UAR(&arx, &generator, &pid))
    , timer(this)
{
    timer.setTimerType(Qt::PreciseTimer); // żeby działał dokładniej
    dziala = dziala.get(); // wywoalanie settera - ustawienie timera
    interwal = interwal.get(); // wywoalanie settera - ustawienie timera
    QObject::connect(&timer, &QTimer::timeout, this, &WarstaUslug::symuluj);
}
void WarstaUslug::uzyjRegluatora(UAR::RodzajSterowania regulacja)
{
    switch (regulacja)
    {
    case UAR::RodzajSterowania::OnOff:
        uar.setOnOff(&onOff);
    case UAR::RodzajSterowania::PID:
        uar.setPID(&pid);
        break;
    }
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
    czas = interwal + czas;
    emit updateCharts(uar.symuluj(), czas);
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
