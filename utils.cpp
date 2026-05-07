#include "utils.hpp"
#include <cassert>

MinMaxClamp::MinMaxClamp(double min, double max, bool active)
    :active(active)
{
    assert(min < max);
    this->max = max;
    this->min = min;
}
bool MinMaxClamp::getActive() const
{
    return active;
}
void MinMaxClamp::setActive(bool active)
{
    this->active = active;
}

void MinMaxClamp::setMin(double value)
{
    assert(value < max);
    this->min = value;
}
void MinMaxClamp::setMax(double value)
{
    assert(min < value);
    this->max = value;
}
double MinMaxClamp::getMin() const
{
    return min;
}
double MinMaxClamp::getMax() const
{
    return max;
}
double MinMaxClamp::clamp(double value)
{
    if(active)
    {
        if(value < min)
            return min;
        else if(value > max)
            return max;
    }
    return value;
}
void MinMaxClamp::setMinMax(double min, double max)
{
    assert(min < max);
    this->max = max;
    this->min = min;
}

