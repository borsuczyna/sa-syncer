#pragma once

class Timer
{
public:
    std::chrono::time_point<std::chrono::steady_clock> m_lastCall;
    int m_interval;
    int m_repeats;
    int m_repeated;
    void(*m_callback)();

    Timer(int interval, int repeats, void(*callback)());
    void Update();
    bool IsFinished();
    void Reset();
    void Kill();
    void Trigger();
};

class CTimers
{
public:
    static std::vector<Timer*> m_timers;

    static void Update();
    static void CreateTimer(int interval, int repeats, void(*callback)());
    static void KillTimer(void(*callback)());
    static void Init();
    static void InitAsync();
};