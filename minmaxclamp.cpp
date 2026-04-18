#include "minmaxclamp.hpp"
#include <cassert>

MinMaxClamp::MinMaxClamp(double min, double max)
{
    assert(min < max);
    this->max = max;
    this->min = min;
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
double MinMaxClamp::getMin()
{
    return min;
}
double MinMaxClamp::getMax()
{
    return max;
}
double MinMaxClamp::clamp(double value)
{
    if(value < min)
    {
        return min;
    }
    else if(value > max)
    {
        return max;
    }
    else
    {
        return value;
    }
}
void MinMaxClamp::setMinMax(double min, double max)
{
    assert(min < max);
    this->max = max;
    this->min = min;
}
