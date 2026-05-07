#include "ListWithWxtremes.hpp"

ListWithExtremes::ListWithExtremes()
    : lista(std::list<std::pair<UAR::Tick, WarstaUslug::Czas>>())
    , val_sterowanie(std::map<double, uint16_t>())
    , val_zadana(std::map<double, uint16_t>())
    , val_regulwoana(std::map<double, uint16_t>())
    , val_uchyb(std::map<double, uint16_t>())
    , val_pid_P(std::map<double, uint16_t>())
    , val_pid_I(std::map<double, uint16_t>())
    , val_pid_D(std::map<double, uint16_t>())
{}

double ListWithExtremes::SterowanieMax()
{
    if(val_sterowanie.size() == 0) return 0.0;
    return this->val_sterowanie.rbegin()->first;
}
double ListWithExtremes::SterowanieMin()
{
    if(val_sterowanie.size() == 0) return 0.0;
    return this->val_sterowanie.begin()->first;
}
double ListWithExtremes::WartoscRegulwoanaMax()
{
    if(val_regulwoana.size() == 0) return 0.0;
    return this->val_regulwoana.rbegin()->first;
}
double ListWithExtremes::WartoscRegulwoanaMin()
{
    if(val_regulwoana.size() == 0) return 0.0;
    return this->val_regulwoana.begin()->first;
}
double ListWithExtremes::WartoscZadanaMax()
{
    if(val_zadana.size() == 0) return 0.0;
    return this->val_zadana.rbegin()->first;
}
double ListWithExtremes::WartoscZadanaMin()
{
    if(val_zadana.size() == 0) return 0.0;
    return this->val_zadana.begin()->first;
}
double ListWithExtremes::UchybMax()
{
    if(val_uchyb.size() == 0) return 0.0;
    return this->val_uchyb.rbegin()->first;
}
double ListWithExtremes::UchybMin()
{
    if(val_uchyb.size() == 0) return 0.0;
    return this->val_uchyb.begin()->first;
}
double ListWithExtremes::PIDPMax()
{
    if(val_pid_P.size() == 0) return 0.0;
    return this->val_pid_P.rbegin()->first;
}
double ListWithExtremes::PIDPMin()
{
    if(val_pid_P.size() == 0) return 0.0;
    return this->val_pid_P.begin()->first;
}
double ListWithExtremes::PIDIMax()
{
    if(val_pid_I.size() == 0) return 0.0;
    return this->val_pid_I.rbegin()->first;
}
double ListWithExtremes::PIDIMin()
{
    if(val_pid_I.size() == 0) return 0.0;
    return this->val_pid_I.begin()->first;
}
double ListWithExtremes::PIDDMax()
{
    if(val_pid_D.size() == 0) return 0.0;
    return this->val_pid_D.rbegin()->first;
}
double ListWithExtremes::PIDDMin()
{
    if(val_pid_D.size() == 0) return 0.0;
    return this->val_pid_D.begin()->first;
}
WarstaUslug::Czas ListWithExtremes::CzasMax()
{
    if(lista.size() == 0) return 0.0;
    return this->lista.front().second;
}
WarstaUslug::Czas ListWithExtremes::CzasMin()
{
    if(lista.size() == 0) return 0.0;
    return this->lista.back().second;
}

void ListWithExtremes::appendLastValue(std::pair<UAR::Tick, WarstaUslug::Czas>& new_point)
{
    lista.push_front(new_point);
    if(this->val_sterowanie.find(new_point.first.sterowanie) == this->val_sterowanie.end())
        this->val_sterowanie[new_point.first.sterowanie] = 0;
    this->val_sterowanie[new_point.first.sterowanie]++;

    if(this->val_regulwoana.find(new_point.first.wartoscRegulowana) == this->val_regulwoana.end())
        this->val_regulwoana[new_point.first.wartoscRegulowana] = 0;
    this->val_regulwoana[new_point.first.wartoscRegulowana]++;

    if(this->val_zadana.find(new_point.first.wartoscZadana) == this->val_zadana.end())
        this->val_zadana[new_point.first.wartoscZadana] = 0;
    this->val_zadana[new_point.first.wartoscZadana]++;

    if(this->val_uchyb.find(new_point.first.uchyb) == this->val_uchyb.end())
        this->val_uchyb[new_point.first.uchyb] = 0;
    this->val_uchyb[new_point.first.uchyb]++;

    if(!new_point.first.pid.has_value())
        // zeby sie nie wykonywalo jak nie ma wartosci dla pida w tym ticku, bo i po co
        return;

    if(this->val_pid_P.find(new_point.first.pid.value().P) == this->val_pid_P.end())
        this->val_pid_P[new_point.first.pid.value().P] = 0;
    this->val_pid_P[new_point.first.pid.value().P]++;

    if(this->val_pid_I.find(new_point.first.pid.value().I) == this->val_pid_I.end())
        this->val_pid_I[new_point.first.pid.value().I] = 0;
    this->val_pid_I[new_point.first.pid.value().I]++;

    if(this->val_pid_D.find(new_point.first.pid.value().D) == this->val_pid_D.end())
        this->val_pid_D[new_point.first.pid.value().D] = 0;
    this->val_pid_D[new_point.first.pid.value().D]++;
}
void ListWithExtremes::deleteFirstValue()
{
    auto to_rm = lista.back();
    this->val_sterowanie[to_rm.first.sterowanie]--;
    this->val_regulwoana[to_rm.first.wartoscRegulowana]--;
    this->val_zadana[to_rm.first.wartoscZadana]--;
    this->val_uchyb[to_rm.first.uchyb]--;

    if(this->val_sterowanie[to_rm.first.sterowanie] == 0)
        this->val_sterowanie.erase(to_rm.first.sterowanie);
    if(this->val_regulwoana[to_rm.first.wartoscRegulowana] == 0)
        this->val_regulwoana.erase(to_rm.first.wartoscRegulowana);
    if(this->val_zadana[to_rm.first.wartoscZadana] == 0)
        this->val_zadana.erase(to_rm.first.wartoscZadana);
    if(this->val_uchyb[to_rm.first.uchyb] == 0)
        this->val_uchyb.erase(to_rm.first.uchyb);

    if(to_rm.first.pid.has_value())
    {
        this->val_pid_P[to_rm.first.pid->P]--;
        this->val_pid_I[to_rm.first.pid->I]--;
        this->val_pid_D[to_rm.first.pid->D]--;

        if(this->val_pid_P[to_rm.first.pid->P] == 0)
            this->val_pid_P.erase(to_rm.first.pid->P);
        if(this->val_pid_I[to_rm.first.pid->I] == 0)
            this->val_pid_I.erase(to_rm.first.pid->I);
        if(this->val_pid_D[to_rm.first.pid->D] == 0)
            this->val_pid_D.erase(to_rm.first.pid->D);
    }

    lista.pop_back();
}
size_t ListWithExtremes::howManyPoints()
{
    return lista.size();
}
void ListWithExtremes::clear()
{
    this->lista.clear();
    this->val_sterowanie.clear();
    this->val_regulwoana.clear();
    this->val_uchyb.clear();
    this->val_zadana.clear();
    this->val_pid_P.clear();
    this->val_pid_I.clear();
    this->val_pid_D.clear();
}
