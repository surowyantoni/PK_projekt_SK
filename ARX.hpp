#pragma once

#include "utils.hpp"
#include "qjsonobject.h"
#include <deque>
#include <vector>

using namespace std;

class ARX
{
private:
    void aktualizacjaBuforowPoZmianieOpoznienia();
    double generujZaklocenie();
public:
    //konstruktory
    ARX(vector<double> nA = {0},
                vector<double> nB = {0},
                int nk = 1,
                double nz = 0);
    MinMaxClamp limityZadana;
    MinMaxClamp limityRegulowana;

    PROPERTY_ACCESS(int, OpoznienieTransportowe)
        void set(const int &k)
        {
            value = k;
            static_cast<ARX*>(owner)->aktualizacjaBuforowPoZmianieOpoznienia();
        }
    } k;



    PROPERTY(double, Zaklocenia)
    } z;

    PROPERTY(deque<double>, Wejscie)
    } U;
    PROPERTY(deque<double>, WyjscieOpoznione)
    } Uopozniony;
    PROPERTY(deque<double>, Wyjscie)
    } Y;

    PROPERTY_ACCESS(vector<double>, WspolczynnikiA)
        void set(const vector<double>& wspolczynniki)
        {
            static_cast<ARX*>(owner)->Y.get().clear();
            value = wspolczynniki;
            for (size_t i = 0; i < value.size(); i++)
                static_cast<ARX*>(owner)->Y.get().push_back(0.0);
        }
    } A;
    PROPERTY_ACCESS(vector<double>, WspolczynnikiB)
        void set(const vector<double>& wspolczynniki)
        {
            static_cast<ARX*>(owner)->U.get().clear();
            value = wspolczynniki;
            for (size_t i = 0; i < value.size(); i++)
                static_cast<ARX*>(owner)->U.get().push_back(0.0);
        }
    } B;

    double symuluj(double u);
    void reset();

    QJsonObject toJSON() const;
    static ARX fromJSON(QJsonObject& json);
};
