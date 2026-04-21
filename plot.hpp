#ifndef PLOT_HPP
#define PLOT_HPP

#include <QObject>
#include <QWidget>

class Plot : public QWidget
{
    Q_OBJECT
public:
    explicit Plot(QWidget *parent = nullptr);

signals:
};

#endif // PLOT_HPP
