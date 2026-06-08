#include "plot.hpp"
#include "qpainter.h"
#include <QPainterPath>

Plot::Plot(ListWithExtremes* lista, WarstaUslug::Czas const * const oknoObserwacji, bool linieCzasu, QWidget *parent)
    : QWidget{parent}
    , siatka(linieCzasu)
    , lista{lista}
    , refreshTimer{QTimer()}
    , oknoObserwacji{oknoObserwacji}
{
    if constexpr (!CONSTS::PLOTS::UPDATE_ON_TICK)
    {
        QObject::connect(&refreshTimer, &QTimer::timeout, this, [this](){
            update();
        });
        refreshTimer.setInterval(CONSTS::PLOTS::UPDATE_INTERVAL);
        refreshTimer.start();
    }
}

inline QPointF Plot::mapPoint(const QPointF &p, const QRectF &src, const QRectF &dst)
{
    double x = dst.left() + (p.x() - src.left()) * dst.width()  / src.width();
    double y = dst.top()  + (p.y() - src.top())  * dst.height() / src.height();
    return QPointF(x, y);
}

inline double mapValue(double min_src, double max_src, double min_dest, double max_dest, double value)
{
    return min_dest + (value - min_src) * (max_dest - min_dest) / (max_src - min_src);
}

void Plot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
    p.setPen(Qt::black);
    QRect caly = rect();


    static const QColor COLOR_ZADANA = QColor::fromRgb(200, 10, 10);
    static const QColor COLOR_REGULOWANA = QColor::fromRgb(10, 200, 10);
    static const QColor COLOR_STEROWANIE = QColor::fromRgb(10, 10, 200);
    static const QColor COLOR_UCHYB = QColor::fromRgb(250, 50, 50);
    static const QColor COLOR_PID_P = QColor::fromRgb(20, 90, 10);
    static const QColor COLOR_PID_I = QColor::fromRgb(100, 10, 200);
    static const QColor COLOR_PID_D = QColor::fromRgb(100, 0, 150);
    constexpr double STOSUNKI_ROZMIARU[] = { 5.0, 2.0, 2.0, 2.0};
    constexpr double STOSUNKI_ROZMIARU_SUMA = 11.0;
    constexpr int ODSTEPY_PX = 20;
    constexpr int MARGINES_WYKRESOW = 10;
    const int OFFSET_LEFT_BRZEGOWE = 30;
    static const QString TYTULY_WYKRESOW[] = {
        "Wartosc zadana i regulowana",
        "Sterowanie",
        "Uchyb",
        "Składowe PID",
    };
    static const QPoint PODZIALKA_Y = QPoint(- OFFSET_LEFT_BRZEGOWE / 2, 0);
    static const QPoint PODZIALKA_X = QPoint(0, OFFSET_LEFT_BRZEGOWE / 2);
    static const QPen GRUBY_PEN = QPen(Qt::BrushStyle::SolidPattern, 2);
    static const QPen CIENKI_PEN = QPen(Qt::BrushStyle::Dense1Pattern, 1);
    constexpr qreal SZEROKOSC_PEDZLA_DLA_TEKSTU_OPISU_SERII = 1.0;
    constexpr int SZEROKOSC_TEKSTU_OPISU_SERII_PX = 200;


    const UAR::Tick MAX_VALUES = {
        .pid = PIDTick{lista->PIDPMax(),
                       lista->PIDIMax(),
                       lista->PIDDMax()},
        .sterowanie = lista->SterowanieMax(),
        .uchyb = lista->UchybMax(),
        .wartoscRegulowana = lista->WartoscRegulwoanaMax(),
        .wartoscZadana = lista->WartoscZadanaMax(),
    };
    const double MAX_PID = std::max({MAX_VALUES.pid->P,  MAX_VALUES.pid->I, MAX_VALUES.pid->D});
    const double MAX_REG_ZAD = std::max(MAX_VALUES.wartoscRegulowana, MAX_VALUES.wartoscZadana);

    const UAR::Tick MIN_VALUES = {
      .pid = PIDTick{lista->PIDPMin(),
                     lista->PIDIMin(),
                     lista->PIDDMin()},
      .sterowanie = lista->SterowanieMin(),
      .uchyb = lista->UchybMin(),
      .wartoscRegulowana = lista->WartoscRegulwoanaMin(),
      .wartoscZadana = lista->WartoscZadanaMin(),
      };
    const double MIN_PID = std::min({MIN_VALUES.pid->P,  MIN_VALUES.pid->I, MIN_VALUES.pid->D});
    const double MIN_REG_ZAD = std::min(MIN_VALUES.wartoscRegulowana, MIN_VALUES.wartoscZadana);


    const std::pair<double, double> BRZEGOWE[] = {
        std::make_pair(MAX_REG_ZAD, MIN_REG_ZAD),
        std::make_pair(MAX_VALUES.sterowanie, MIN_VALUES.sterowanie),
        std::make_pair(MAX_VALUES.uchyb, MIN_VALUES.uchyb),
        std::make_pair(MAX_PID, MIN_PID),
    };



    const QRect ramka_zadana_i_reg =
        QRect(caly.topLeft().x() + ODSTEPY_PX + OFFSET_LEFT_BRZEGOWE,
              caly.topLeft().y() + ODSTEPY_PX,
              caly.width() - ODSTEPY_PX * 2 - OFFSET_LEFT_BRZEGOWE,
              (caly.height() - ODSTEPY_PX * 5) / STOSUNKI_ROZMIARU_SUMA * STOSUNKI_ROZMIARU[0]);
    const QRect ramka_sterowanie =
        QRect(caly.topLeft().x() + ODSTEPY_PX + OFFSET_LEFT_BRZEGOWE,
              ramka_zadana_i_reg.bottomLeft().y() + ODSTEPY_PX,
              caly.width() - ODSTEPY_PX * 2 - OFFSET_LEFT_BRZEGOWE,
              (caly.height() - ODSTEPY_PX * 5) / STOSUNKI_ROZMIARU_SUMA * STOSUNKI_ROZMIARU[1]);
    const QRect ramka_uchyb =
        QRect(caly.topLeft().x() + ODSTEPY_PX + OFFSET_LEFT_BRZEGOWE,
              ramka_sterowanie.bottomLeft().y() + ODSTEPY_PX,
              caly.width() - ODSTEPY_PX * 2 - OFFSET_LEFT_BRZEGOWE,
              (caly.height() - ODSTEPY_PX * 5) / STOSUNKI_ROZMIARU_SUMA * STOSUNKI_ROZMIARU[2]);
    const QRect ramka_pid =
        QRect(caly.topLeft().x() + ODSTEPY_PX + OFFSET_LEFT_BRZEGOWE,
              ramka_uchyb.bottomLeft().y() + ODSTEPY_PX,
              caly.width() - ODSTEPY_PX * 2 - OFFSET_LEFT_BRZEGOWE,
              (caly.height() - ODSTEPY_PX * 5) / STOSUNKI_ROZMIARU_SUMA * STOSUNKI_ROZMIARU[3]);

    const QRect* RAMKI[] = {
        &ramka_zadana_i_reg,
        &ramka_sterowanie,
        &ramka_uchyb,
        &ramka_pid
    };

    int idx_ramki = 0;
    for(auto ramka : RAMKI)
    {
        const_cast<QBrush&>(p.brush()).setColor(Qt::gray);
        p.setPen(GRUBY_PEN);
        p.drawRect(*ramka);

        p.save();
        p.translate(ramka->bottomLeft());
        p.rotate(-90);
        p.drawText(QRect(0, 0, ramka->height(), -ODSTEPY_PX), Qt::AlignCenter, TYTULY_WYKRESOW[idx_ramki]);
        p.restore();
        p.drawLine(ramka->topLeft() + QPoint(0, MARGINES_WYKRESOW), ramka->topLeft() + PODZIALKA_Y + QPoint(0, MARGINES_WYKRESOW));
        p.drawLine(ramka->bottomLeft() + QPoint(0, -MARGINES_WYKRESOW), ramka->bottomLeft() + PODZIALKA_Y + QPoint(0, -MARGINES_WYKRESOW));

        p.drawText(ramka->topLeft() + PODZIALKA_Y + QPoint(-OFFSET_LEFT_BRZEGOWE , ODSTEPY_PX+MARGINES_WYKRESOW), QString::number(BRZEGOWE[idx_ramki].first, 10, 3));
        p.drawText(ramka->bottomLeft() + PODZIALKA_Y + QPoint(-OFFSET_LEFT_BRZEGOWE , -ODSTEPY_PX-MARGINES_WYKRESOW), QString::number(BRZEGOWE[idx_ramki].second, 10, 3));

        idx_ramki++;
    }

    //Opisy serii
    p.drawText(ramka_zadana_i_reg.center().x(), ramka_zadana_i_reg.top() - (ODSTEPY_PX / 4), "Czas [s]");

    p.setFont(QFont("Soege UI", 9, 600));
    p.setPen(QPen(QBrush(COLOR_ZADANA), SZEROKOSC_PEDZLA_DLA_TEKSTU_OPISU_SERII));
    p.drawText(ramka_zadana_i_reg.topRight() + QPoint(-SZEROKOSC_TEKSTU_OPISU_SERII_PX, ODSTEPY_PX * 2 ), "Wartość zadana");
    p.setPen(QPen(QBrush(COLOR_REGULOWANA), SZEROKOSC_PEDZLA_DLA_TEKSTU_OPISU_SERII));
    p.drawText(ramka_zadana_i_reg.topRight() + QPoint(-SZEROKOSC_TEKSTU_OPISU_SERII_PX, ODSTEPY_PX * 3), "Wartość regulowana");


    p.setPen(QPen(QBrush(COLOR_STEROWANIE), SZEROKOSC_PEDZLA_DLA_TEKSTU_OPISU_SERII));
    p.drawText(ramka_sterowanie.topRight() + QPoint(-SZEROKOSC_TEKSTU_OPISU_SERII_PX, ODSTEPY_PX * 2), "Sterowanie");

    p.setPen(QPen(QBrush(COLOR_UCHYB), SZEROKOSC_PEDZLA_DLA_TEKSTU_OPISU_SERII));
    p.drawText(ramka_uchyb.topRight() + QPoint(-SZEROKOSC_TEKSTU_OPISU_SERII_PX, ODSTEPY_PX * 2), "Uchyb");

    p.setPen(QPen(QBrush(COLOR_PID_P), SZEROKOSC_PEDZLA_DLA_TEKSTU_OPISU_SERII));
    p.drawText(ramka_pid.topRight() + QPoint(-SZEROKOSC_TEKSTU_OPISU_SERII_PX, ODSTEPY_PX * 2), "Część proporcjonalna (P)");
    p.setPen(QPen(QBrush(COLOR_PID_I), SZEROKOSC_PEDZLA_DLA_TEKSTU_OPISU_SERII));
    p.drawText(ramka_pid.topRight() + QPoint(-SZEROKOSC_TEKSTU_OPISU_SERII_PX, ODSTEPY_PX * 3), "Część integracyjna   (I)");
    p.setPen(QPen(QBrush(COLOR_PID_D), SZEROKOSC_PEDZLA_DLA_TEKSTU_OPISU_SERII));
    p.drawText(ramka_pid.topRight() + QPoint(-SZEROKOSC_TEKSTU_OPISU_SERII_PX, ODSTEPY_PX * 4), "Część rożniczkująca  (D)");




    if(lista->howManyPoints() == 0) return; // nie rysuj pustych wykresów

    if (lista->howManyPoints() <= 2) return;
    auto punkt =  lista->lista.rbegin();
    unsigned int czas_on_plot = mapValue(lista->CzasMin(), lista->CzasMax(), ramka_zadana_i_reg.left(), ramka_zadana_i_reg.right(), punkt->second);

    QPoint last_sterowanie = QPoint(czas_on_plot, mapValue(MIN_VALUES.sterowanie, MAX_VALUES.sterowanie, ramka_sterowanie.bottom() - MARGINES_WYKRESOW, ramka_sterowanie.top() + MARGINES_WYKRESOW, punkt->first.sterowanie));
    QPoint last_uchyb = QPoint(czas_on_plot, mapValue(MIN_VALUES.uchyb, MAX_VALUES.uchyb, ramka_uchyb.bottom() -MARGINES_WYKRESOW, ramka_uchyb.top()+MARGINES_WYKRESOW, punkt->first.uchyb));
    QPoint last_regulowana = QPoint(czas_on_plot, mapValue(MIN_REG_ZAD, MAX_REG_ZAD, ramka_zadana_i_reg.bottom()-MARGINES_WYKRESOW, ramka_zadana_i_reg.top()+MARGINES_WYKRESOW, punkt->first.wartoscRegulowana));
    QPoint last_zadana = QPoint(czas_on_plot, mapValue(MIN_REG_ZAD, MAX_REG_ZAD, ramka_zadana_i_reg.bottom()-MARGINES_WYKRESOW, ramka_zadana_i_reg.top()+MARGINES_WYKRESOW, punkt->first.wartoscZadana));
    QPoint last_pid_P;
    QPoint last_pid_I;
    QPoint last_pid_D;
    unsigned int last_czas = punkt->second;


    bool last_pid = punkt->first.pid.has_value();
    if(last_pid)
    {
        last_pid_P = QPoint(czas_on_plot, mapValue(MIN_PID, MAX_PID, ramka_pid.bottom()-MARGINES_WYKRESOW, ramka_pid.top()+MARGINES_WYKRESOW, punkt->first.pid->P));
        last_pid_I = QPoint(czas_on_plot, mapValue(MIN_PID, MAX_PID, ramka_pid.bottom()-MARGINES_WYKRESOW, ramka_pid.top()+MARGINES_WYKRESOW, punkt->first.pid->I));
        last_pid_D = QPoint(czas_on_plot, mapValue(MIN_PID, MAX_PID, ramka_pid.bottom()-MARGINES_WYKRESOW, ramka_pid.top()+MARGINES_WYKRESOW, punkt->first.pid->D));
    }
    static const constexpr int SZEROKOSC_LINII = 2;



    QPainterPath pathSterowanie;
    QPainterPath pathUchyb;
    QPainterPath pathRegulowana;
    QPainterPath pathZadana;
    QPainterPath pathPidP;
    QPainterPath pathPidI;
    QPainterPath pathPidD;
    pathSterowanie.moveTo(last_sterowanie);
    pathUchyb.moveTo(last_uchyb);
    pathRegulowana.moveTo(last_regulowana);
    pathZadana.moveTo(last_zadana);
    if(last_pid)
    {
        pathPidP.moveTo(last_pid_P);
        pathPidI.moveTo(last_pid_I);
        pathPidD.moveTo(last_pid_D);
    }




    //Rysowanie czasu
    for (uint32_t sekunda = punkt->second / 1000 + 1; sekunda <= lista->lista.begin()->second / 1000; ++sekunda)
    {
        QPoint spot(mapValue(lista->CzasMin(), lista->CzasMax(), ramka_zadana_i_reg.left(), ramka_zadana_i_reg.right(), sekunda * 1000), ramka_pid.bottom() + SZEROKOSC_LINII);
        p.drawLine(spot, spot + PODZIALKA_X);
        p.drawText(spot + QPoint(3, OFFSET_LEFT_BRZEGOWE / 3 * 2), QString::number(sekunda));

        if(siatka.get())
        {
            p.setPen(CIENKI_PEN);
            for (int idx = 0; idx < 4; ++idx)
            {
                p.drawLine(QPoint(spot.x(), RAMKI[idx]->bottom()), QPoint(spot.x(), RAMKI[idx]->top()));
            }
            p.setPen(GRUBY_PEN);
        }
    }



    // UNDERSAMPLING
    size_t step = lista->howManyPoints() / CONSTS::PLOTS::WSPOLCZYNNIK_OPTYALIZACJIL;
    if(step == 0)
        step = 1;

#ifdef DEBUG
    // qDebug() << "Rysuję co " << step << " punktów";
#endif // DEBUG

    for (size_t idx = step; idx < lista->howManyPoints() - 1; idx += step)
    {

        czas_on_plot = mapValue(lista->CzasMin(), lista->CzasMax(), ramka_zadana_i_reg.left(), ramka_zadana_i_reg.right(), punkt->second);
        QPoint sterowanie = QPoint(czas_on_plot, mapValue(MIN_VALUES.sterowanie, MAX_VALUES.sterowanie, ramka_sterowanie.bottom() - MARGINES_WYKRESOW, ramka_sterowanie.top() + MARGINES_WYKRESOW, punkt->first.sterowanie));
        QPoint uchyb = QPoint(czas_on_plot, mapValue(MIN_VALUES.uchyb, MAX_VALUES.uchyb, ramka_uchyb.bottom()-MARGINES_WYKRESOW, ramka_uchyb.top()+MARGINES_WYKRESOW, punkt->first.uchyb));
        QPoint regulowana = QPoint(czas_on_plot, mapValue(MIN_REG_ZAD, MAX_REG_ZAD, ramka_zadana_i_reg.bottom()-MARGINES_WYKRESOW, ramka_zadana_i_reg.top()+MARGINES_WYKRESOW, punkt->first.wartoscRegulowana));
        QPoint zadana = QPoint(czas_on_plot, mapValue(MIN_REG_ZAD, MAX_REG_ZAD, ramka_zadana_i_reg.bottom()-MARGINES_WYKRESOW, ramka_zadana_i_reg.top()+MARGINES_WYKRESOW, punkt->first.wartoscZadana));
        QPoint pid_P;
        QPoint pid_I;
        QPoint pid_D;
        if(punkt->first.pid.has_value())
        {
            pid_P = QPoint(czas_on_plot, mapValue(MIN_PID, MAX_PID, ramka_pid.bottom()-MARGINES_WYKRESOW, ramka_pid.top()+MARGINES_WYKRESOW, punkt->first.pid->P));
            pid_I = QPoint(czas_on_plot, mapValue(MIN_PID, MAX_PID, ramka_pid.bottom()-MARGINES_WYKRESOW, ramka_pid.top()+MARGINES_WYKRESOW, punkt->first.pid->I));
            pid_D = QPoint(czas_on_plot, mapValue(MIN_PID, MAX_PID, ramka_pid.bottom()-MARGINES_WYKRESOW, ramka_pid.top()+MARGINES_WYKRESOW, punkt->first.pid->D));
        }

        pathSterowanie.lineTo(sterowanie);
        pathUchyb.lineTo(uchyb);
        pathRegulowana.lineTo(regulowana);
        pathZadana.lineTo(zadana);
        if(punkt->first.pid.has_value())
        {
            if(last_pid)
            {
                pathPidP.lineTo(pid_P);
                pathPidI.lineTo(pid_I);
                pathPidD.lineTo(pid_D);
            }
            else
            {
                pathPidP.moveTo(pid_P);
                pathPidI.moveTo(pid_I);
                pathPidD.moveTo(pid_D);
            }
        }


        // last_sterowanie = sterowanie;
        // last_uchyb = uchyb;
        // last_regulowana = regulowana;
        // last_zadana = zadana;
        // last_czas = punkt->second;
        last_pid = punkt->first.pid.has_value();
        if(punkt->first.extrapolated)
        {
            p.drawEllipse(sterowanie, 3, 3);
            p.drawEllipse(regulowana, 3, 3);
            p.drawEllipse(zadana, 3, 3);
        }
        std::advance(punkt, step);
    }


    p.setPen(QPen(QBrush(COLOR_STEROWANIE), SZEROKOSC_LINII));
    p.drawPath(pathSterowanie);
    p.setPen(QPen(QBrush(COLOR_UCHYB), SZEROKOSC_LINII));
    p.drawPath(pathUchyb);
    p.setPen(QPen(QBrush(COLOR_REGULOWANA), SZEROKOSC_LINII));
    p.drawPath(pathRegulowana);
    p.setPen(QPen(QBrush(COLOR_ZADANA), SZEROKOSC_LINII));
    p.drawPath(pathZadana);
    p.setPen(QPen(QBrush(COLOR_PID_P), SZEROKOSC_LINII));
    p.drawPath(pathPidP);
    p.setPen(QPen(QBrush(COLOR_PID_I), SZEROKOSC_LINII));
    p.drawPath(pathPidI);
    p.setPen(QPen(QBrush(COLOR_PID_D), SZEROKOSC_LINII));
    p.drawPath(pathPidD);
}


