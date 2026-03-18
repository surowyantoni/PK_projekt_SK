#pragma once
#include <vector>
using namespace std;

class GenWar
{
    static std::vector<double> Generate(int Tt, int Tr, int A, int S, int c, double p = 0.5);

private:
    GenWar() = default;
};
