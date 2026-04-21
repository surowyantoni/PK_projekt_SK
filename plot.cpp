#include "plot.hpp"
#include "qpainter.h"

Plot::Plot(QWidget *parent)
    : QWidget{parent}
{}

static inline QPointF mapPoint(const QPointF &p, const QRectF &src, const QRectF &dst)
{
    double x = dst.left() + (p.x() - src.left()) * dst.width()  / src.width();
    double y = dst.top()  + (p.y() - src.top())  * dst.height() / src.height();
    return QPointF(x, y);
}

void Plot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    if(serie.size() == 0) return;
    if(serie[0].dane->getList()->size() == 0) return;
    p.setPen(Qt::black);
    QRect widget = rect();

    double maxWart = serie[0].dane->max();
    double minWart = serie[0].dane->min();


    double szerokosc = serie[0].dane->getList()->last().x() - serie[0].dane->getList()->first().x();

    if(szerokosc == 0) return;
    int offset = widget.width() / szerokosc;
    for (int var = 0; var < szerokosc; var++)
    {
        p.drawLine(widget.topLeft() + QPoint(var * offset , 0), widget.bottomLeft() +QPoint(var * offset , 0));
        p.drawText(widget.bottomLeft() + QPoint(var * offset , -20), QString::number(offset));
    }

    p.drawLine(widget.topLeft() + QPoint(5, 5), widget.topRight() + QPoint(-5, 5));
    p.drawText(widget.topLeft() + QPoint(5, 5 + 12), QString::number(minWart));
    p.drawLine(widget.bottomLeft() + QPoint(5, -16), widget.bottomRight() + QPoint(-5, -16));
    p.drawText(widget.bottomLeft() + QPoint(5, -(8 + 12)), QString::number(maxWart));

    if(minWart < 0.0 && 0.0 < maxWart)
    {
        // p.drawLine(widget.center() + QPoint(5, -16), widget.bottomRight() + QPoint(-5, -16));
        // p.drawText(widget.bottomLeft() + QPoint(5, -(8 + 12)), "0.0");
    }

    for (int i = 0; i < serie.size(); ++i)
    {
        auto& seria = serie[i];
        p.setRenderHint(QPainter::TextAntialiasing);
        p.setPen(Qt::black);

        QFont font("Arial", 9);
        p.setFont(font);
        p.drawText(widget.bottomRight() + QPointF(-200, -12 * (i+1) - 10), seria.nazwa);


        p.fillRect(QRect(widget.bottomRight() + QPoint(-210, -12 * (i+1) - 20), QSize(8, 8)), seria.kolor);
        p.drawRect(QRect(widget.bottomRight() + QPoint(-210, -12 * (i+1) - 20), QSize(8, 8)));
        QRectF przestrzenWykresu(serie[0].dane->getList()->first().x(), serie[0].dane->min(), szerokosc, wysokosc);

        // qDebug() << przestrzenWykresu.width();
        // qDebug() << "H" << przestrzenWykresu.height();
        // p.drawPoint(mapPoint(point, przestrzenWykresu, widget));

        QVector<QPointF> punkty;
        punkty.reserve(seria.dane->getList()->size());
        for(auto& point : *seria.dane->getList())
        {
            punkty.append(mapPoint(point, przestrzenWykresu, widget));
        }
        p.setPen(seria.kolor);
        for(size_t i = 0; i < punkty.size() - 1; i++)
        {
            p.drawLine(punkty[i], punkty[i + 1]);
            // qDebug() << mapPoint(point, przestrzenWykresu, widget);
        }
        minWart = std::min(minWart, seria.dane->min());
        maxWart = std::max(maxWart, seria.dane->max());

    }
    wysokosc = maxWart - minWart;
}


