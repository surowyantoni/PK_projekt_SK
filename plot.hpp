#ifndef PLOT_HPP
#define PLOT_HPP

#include "ListWithWxtremes.hpp"
#include <QObject>
#include <QWidget>

struct Seria
{
    ListWithExtremes* dane;
    QColor kolor;
    QString nazwa;
};

class Plot : public QWidget
{
    Q_OBJECT
public:
    explicit Plot(QWidget *parent = nullptr);
    std::vector<Seria> serie = std::vector<Seria>();
signals:
protected:
    void paintEvent(QPaintEvent *event) override;
    double wysokosc = 1.0;
};

#endif // PLOT_HPP
