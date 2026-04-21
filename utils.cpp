#include "utils.hpp"
#include <cassert>

MinMaxClamp::MinMaxClamp(double min, double max, bool active)
    :active(active)
{
    assert(min < max);
    this->max = max;
    this->min = min;
}
bool MinMaxClamp::getActive()
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
template<typename T>
Property<T>::Property(T& initialValue)
    :value(initialValue)
{}

template<typename T>
void Property<T>::set(const T &value)
{
    this->value = value;
}

template<typename T>
T Property<T>::get() const
{
    return this->value;
}

template<typename T>
void Property<T>::operator=(const T &value)
{
    set(value);
}

template<typename T>
Property<T>::operator T() const
{
    return get();
}

template<typename T>
PropertyWithAccess<T>::PropertyWithAccess(void* owner)
    : owner(owner)
{}
template<typename T>
PropertyWithAccess<T>::PropertyWithAccess(void* owner, T &initalValue)
    : Property<T>(initalValue)
    , owner(owner)
{}

