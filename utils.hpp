#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
class MinMaxClamp
{
    double min;
    double max;
    bool active;
public:
    MinMaxClamp(double min, double max, bool active = true);
    double clamp(double value);
    void setMinMax(double min, double max);
    void setMin(double value);
    void setMax(double value);
    void setActive(bool active);
    double getMin();
    double getMax();
    bool getActive();
};

template<typename T>
class Property
{
protected:
    T value;
public:
    Property<T>() = default;
    Property<T>(T& initialValue)
    :value(initialValue)
    {

    }
    Property<T> (Property<T>&) = delete;
    Property<T> (Property<T>&&) = delete;
    Property<T> operator=(Property<T>&) = delete;
    Property<T> operator=(Property<T>&&) = delete;

    // to do override
    void set(const T& value)
    {  this->value = value; }
    void operator=(const T& value)
    { set(value); }
    // to do override
    T get() const
    {   return value;   }
    operator T() const
    { return get(); }
};

template<typename T>
class PropertyWithAccess : public Property<T>
{
protected:
    T value;
    void* owner;
public:
    PropertyWithAccess<T>() = delete;
    PropertyWithAccess<T>(void* owner, T &initalValue)
        : Property<T>(initalValue)
        , owner(owner)
    {}
    PropertyWithAccess<T> (PropertyWithAccess<T>&) = delete;
    PropertyWithAccess<T> (PropertyWithAccess<T>&&) = delete;
    PropertyWithAccess<T> operator=(PropertyWithAccess<T>&) = delete;
    PropertyWithAccess<T> operator=(PropertyWithAccess<T>&&) = delete;
};

#define PROPERTY(type, name) struct name : public Property<type>\
{\
    name(type& value)\
    : Property<type>(value) {}\
    name(type&& value)\
    : Property<type>(value) {}\
    void operator=(const type&& value) { set(value); }\
    operator type() const { return get(); }


#define PROPERTY_ACCESS(type, name) \
struct name : public PropertyWithAccess<type>\
{ \
    name(void* owner, type& value)\
    : PropertyWithAccess<type>(owner, value) {}\
    name(void* owner, type&& value)\
    : PropertyWithAccess<type>(owner, value) {}\
    void operator=(const type&& value) { set(value); }\
    operator type() const { return get(); }


// USAGE
// class car
// {
// protected:
//     int asd;
// public:
//     car() {}
//     struct : public Property<int> {
//         void set(int a)
//         {
//             value = (a < 0 ? 0 : a);
//         }
//     } weight;
//     struct : public Property<int> {
//         void set(int a)
//         {
//             value = (a > 100 ? a : 100);
//         }
//     } speed;
// };

inline uint32_t secondsToMili(double seconds);
inline double miliToSeconds(uint32_t miliseconds);


#endif // UTILS_H
