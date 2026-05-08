#ifndef LISTWITHWXTREMES_H
#define LISTWITHWXTREMES_H
#include <qlist.h>
#include <qpoint.h>
#include <map>
#include "UAR.h"
#include "WarstwaUslug.h"


/**
 * @brief Klasa przechowująca dane do wyświetlenie na wykresach. Pozwala na odszukanie i zwrócenie najmniejsze i największej wartości w stałycm czasie
 *
 */
class ListWithExtremes
{
    friend class Plot;
    std::list<std::pair<UAR::Tick, WarstaUslug::Czas>> lista;
    std::map<double, uint16_t> val_sterowanie;
    std::map<double, uint16_t> val_zadana;
    std::map<double, uint16_t> val_regulwoana;
    std::map<double, uint16_t> val_uchyb;
    std::map<double, uint16_t> val_pid_P;
    std::map<double, uint16_t> val_pid_I;
    std::map<double, uint16_t> val_pid_D;

public:
    ListWithExtremes();
    double SterowanieMax();
    double SterowanieMin();
    double WartoscRegulwoanaMax();
    double WartoscRegulwoanaMin();
    double WartoscZadanaMax();
    double WartoscZadanaMin();
    double UchybMax();
    double UchybMin();
    double PIDPMax();
    double PIDPMin();
    double PIDIMax();
    double PIDIMin();
    double PIDDMax();
    double PIDDMin();
    WarstaUslug::Czas CzasMax();
    WarstaUslug::Czas CzasMin();
    size_t howManyPoints();
    void appendLastValue(std::pair<UAR::Tick, WarstaUslug::Czas>& new_point);
    void deleteFirstValue();
    WarstaUslug::Czas timeWidth();
    void clear();
};

#endif // LISTWITHWXTREMES_H
