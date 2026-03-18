#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

class Timer
{
    std::atomic<bool> active{true};

public:
    template<typename Function>                        //me
    void setTimeout(Function function, int delay);     //było auto function
    template<typename Function>                        //me
    void setInterval(Function function, int interval); //było auto function
    void stop();
};

template<typename Function> //me
void Timer::setTimeout(Function function, int delay)
{ //było auto function
    active = true;
    std::thread t([=]() {
        if (!active.load())
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if (!active.load())
            return;
        function();
    });
    t.detach();
}

template<typename Function> //me
void Timer::setInterval(Function function, int interval)
{ //było auto function
    active = true;
    std::thread t([=]() {
        while (active.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if (!active.load())
                return;
            function();
        }
    });
    t.detach();
}

inline void Timer::stop()
{
    active = false;
}
