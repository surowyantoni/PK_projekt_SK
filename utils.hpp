#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <utility>
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
    double getMin() const;
    double getMax() const;
    bool getActive() const;
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
    const T get() const
    {   return value;   }
    operator const T() const
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

#define PROPERTY(type) struct : public Property<type>\
{\
    void operator=(const type& value) { set(value); }\
    operator const type() const { return get(); }\
    type& use() { return value; }\


#define PROPERTY_ACCESS(type) \
struct : public PropertyWithAccess<type>\
{ \
    void operator=(const type& value) { set(value); }\
    operator const type() const { return get(); }\
    type& use() { return value; }

#define PROP(type, ownerType) \
struct { \
type value; \
ownerType* owner;


#define SETTER(type) \
void set(const type& value) \
{  this->value = value; }

#define GETTER(type) \
const type& get() const \
{  return this->value; }

// #define OPERATORS(type) \
// operator const type&() \
// { return get(); } \
// void operator=(const type& value) \
// { set(value); }

#define DEFAULTS(type) \
SETTER(type) \
GETTER(type) \
// OPERATORS(type)



// #define OWNER(type) \
// type* owner = static_cast<type*>(this->owner);


// class car {
//     PROP(int)
//         SETTER(int)
//         GETTER(int)
//         OPERATORS(int)
//     } engine_miles;
//     car();
// };

// car::car()
//     : engine_miles{100, this}
// {
//     engine_miles = 4;
// }


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

#define SERIALIZABLE \
QJsonObject toJSON() const; \
void fromJSON(QJsonObject& json); \
QByteArray toByteArray() const; \
void fromByteArray(QByteArray& data);



/**

QJsonObject CLASS::toJSON() const
{

}
void CLASS::fromJSON(QJsonObject& json)
{

}
QByteArray CLASS::toByteArray() const
{

}
void CLASS::fromByteArray(QByteArray& data)
{

}

*/

// convert Seconds to miliseconds
inline uint32_t secondsToMili(double seconds)
{
    return seconds * 1000;
}
inline double miliToSeconds(uint32_t miliseconds)
{
    return (double)miliseconds / 1000.0;
}


#endif // UTILS_H
