#ifndef PARAMETRYARXWINDOW_H
#define PARAMETRYARXWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include "WarstwaUslug.h"
#include "qspinbox.h"


namespace Ui {
class ParametryARXWindow;
}

class ParametryARXWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ParametryARXWindow(WarstaUslug& uslugi);

    ~ParametryARXWindow();
signals:
    void closed();
public:
    void updateUI();
private slots:


    void on_pushButton_addWspolczynnik_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    virtual void closeEvent(QCloseEvent* event) override;

    QList<std::pair<QDoubleSpinBox*, QDoubleSpinBox*>> wspolczynniki;

    void dodajPareWspolczynnikow(double a = 0.0, double b = 0.0);

    Ui::ParametryARXWindow *ui;

    WarstaUslug& uslugi;
};

#endif // PARAMETRYARXWINDOW_H
