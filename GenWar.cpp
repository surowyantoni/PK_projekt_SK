#include "GenWar.h"
#include <cmath>
const double PI = 3.14159265358979323846;

std::vector<double> GenWar::Generate(int Tt, int Tr, int A, int S, int c, double p)
{
    vector<double> w;
    if (p > 1.0) {
        p = 1;
    }
    if (p < 0.0) {
        p = 0;
    }
    int choice = c;
    int T = floor(Tr / (Tt / 1000.0));

    double tempSin;
    double tempW;
    switch (choice) {
    case 1: //sinus
        for (int i = 1; i <= T; i++) {
            tempSin = i % T;
            tempSin /= T;
            tempSin *= 2 * PI;
            tempW = A * sin(tempSin) + S;
            w.push_back(tempW);
        }
        break;
    case 2: //kwadrat
        for (int i = 1; i <= T; i++) {
            if (i % T < p * T)
                w.push_back(A + S);
            else
                w.push_back(S);
        }
        break;
    }

    /*for (int i = 0; i < T; i++)
		cout << w.at(i) << "\n";*/

    return w;
}
