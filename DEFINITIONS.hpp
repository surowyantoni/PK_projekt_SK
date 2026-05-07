#pragma once
#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP
#include <array>

// ustaw na MAIN aby skompilować program docelowy
// ustaw na DEBUG aby skompilować program testujacy
#define MAIN
// #define DEBUG


namespace CONSTS
{
    namespace NET
    {
        constexpr int DISCOVERY_PORT = 5001;
        constexpr int MAX_AUTH_ATTEMPTS = 3;
    }
    namespace ARX
    {
        constexpr int k = 1;
        constexpr double z = 0.0;
        constexpr std::array<double, 1> A = { -0.4 };
        constexpr std::array<double, 1> B = { 0.6 };
        constexpr double U_min = -10.0;
        constexpr double U_max = 10.0;
        constexpr bool U_active = true;
        constexpr double Y_min = -10.0;
        constexpr double Y_max = 10.0;
        constexpr bool Y_active = true;
    }
    namespace PID
    {
        constexpr double P = 0.5;
        constexpr double I = 5.0;
        constexpr double D = 0.0;
        constexpr int rodzaj_calkowania = 0; //::RegulatorPID::Zewnetrzne
        constexpr double U_min = -10.0;
        constexpr double U_max = 10.0;
        constexpr bool U_active = true;
        constexpr bool antiWindupActive = true;
    }
    namespace UAR
    {
        constexpr int regulator = 1; //::UAR::RodzajSterowania::PID;
        constexpr uint32_t interwal = 50; // milisekund
        constexpr bool started = false;

    }
    namespace Generator
    {
        constexpr double amplituda = 1.0;
        constexpr double wypelnienie = 0.5;
        constexpr double okres = 10.0; // sekund
        constexpr double skladowaStala = 0.0;
        constexpr int sygnal = 0; // ::GeneratorWartosci::TypSygnalu::KWADRAT;
    }
    namespace OnOff
    {
        constexpr double sterowanie = 2.0;
        constexpr double histereza = 0.1;
    }
    namespace GUI
    {
        constexpr double oknoObserwacji = 10.0;

        constexpr double oknoObserwacji_min =  5.0;
        constexpr double oknoObserwacji_max =  50.0;
        constexpr double oknoObserwacji_step=  5.0;
        namespace UAR
        {
            constexpr int interwal_min =  10;
            constexpr int interwal_max =  500;
            constexpr int interwal_step=  10;
        }
        namespace ARX
        {
            constexpr double A_min = -2.0;
            constexpr double A_max =  2.0;
            constexpr double A_step=  0.1;

            constexpr double B_min = -2.0;
            constexpr double B_max =  2.0;
            constexpr double B_step=  0.1;

            constexpr int k_min = 1;
            constexpr int k_max = 20;
            constexpr int k_step= 1;

            constexpr double z_min =  0.0;
            constexpr double z_max =  0.1;
            constexpr double z_step=  0.01;

            constexpr double U_min = -100.0;
            constexpr double U_max =  100.0;
            constexpr double U_step=  1.0;

            constexpr double Y_min = -100.0;
            constexpr double Y_max =  100.0;
            constexpr double Y_step=  1.0;
        }
        namespace PID
        {
            constexpr double P_min =  0.0;
            constexpr double P_max =  100.0;
            constexpr double P_step=  0.1;

            constexpr double I_min =  0.0;
            constexpr double I_max =  100.0;
            constexpr double I_step=  1.0;

            constexpr double D_min =  0.0;
            constexpr double D_max =  100.0;
            constexpr double D_step=  0.1;

            constexpr double U_min = -100.0;
            constexpr double U_max =  100.0;
            constexpr double U_step=  1.0;
        }
        namespace Generator
        {
            constexpr double amplituda_min =  0.0;
            constexpr double amplituda_max =  100.0;
            constexpr double amplituda_step=  1.0;

            constexpr double okres_min =  0.0;
            constexpr double okres_max =  100.0;
            constexpr double okres_step=  1.0;

            constexpr double wypelnienie_min =  0.0;
            constexpr double wypelnienie_max =  1.0;
            constexpr double wypelnienie_step=  0.1;

            constexpr double skladowaStala_min =  -100.0;
            constexpr double skladowaStala_max =  100.0;
            constexpr double skladowaStala_step=  1.0;
        }
        namespace OnOff
        {
            constexpr double sterowanie_min =  0.0;
            constexpr double sterowanie_max =  100.0;
            constexpr double sterowanie_step=  0.5;

            constexpr double histereza_min =  0.05;
            constexpr double histereza_max =  10.0;
            constexpr double histereza_step=  0.05;
        }
    }
}
#endif // DEFINITIONS_HPP
