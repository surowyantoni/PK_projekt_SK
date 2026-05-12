#ifndef PLOT_HPP
#define PLOT_HPP

#include "ListWithWxtremes.hpp"
#include <QObject>
#include <QWidget>

class Plot : public QWidget
{
    Q_OBJECT
    static inline QPointF mapPoint(const QPointF &p, const QRectF &src, const QRectF &dst);
public:
    explicit Plot(ListWithExtremes* lista, WarstaUslug::Czas const * const oknoObserwacji, bool linieCzasu = false, QWidget *parent = nullptr);
    PROP(qint64, Plot)
        GETTER(qint64)
        void set(const qint64& refresh)
        {
            owner->refreshTimer.setInterval(refresh);
            this->value = refresh;
        }
    } refreshInterval;
    PROP(bool, Plot)
        GETTER(bool)
        SETTER(bool)
    } siatka;
protected:
    void paintEvent(QPaintEvent *event) override;
    ListWithExtremes* lista;
    QTimer refreshTimer;
    WarstaUslug::Czas const * const oknoObserwacji;
};

#endif // PLOT_HPP
