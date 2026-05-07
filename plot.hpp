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
signals:
protected:
    void paintEvent(QPaintEvent *event) override;
    ListWithExtremes* lista;
};

#endif // PLOT_HPP
