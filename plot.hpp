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
    explicit Plot(ListWithExtremes* lista, QWidget *parent = nullptr);
    PROP(qint64, Plot)
        GETTER(qint64)
        void set(const qint64& refresh)
        {
            owner->refreshTimer.setInterval(refresh);
            this->value = refresh;
        }
    } refreshInterval;
protected:
    void paintEvent(QPaintEvent *event) override;
    ListWithExtremes* lista;
    QTimer refreshTimer;
};

#endif // PLOT_HPP
