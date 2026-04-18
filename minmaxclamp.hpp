#ifndef MINMAXCLAMP_HPP
#define MINMAXCLAMP_HPP

class MinMaxClamp
{
    double min;
    double max;
public:
    MinMaxClamp(double min, double max);
    double clamp(double value);
    void setMinMax(double min, double max);
    void setMin(double value);
    void setMax(double value);
    double getMin();
    double getMax();
};

#endif // MINMAXCLAMP_HPP
