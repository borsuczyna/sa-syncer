#include "../stdafx.h"

std::vector<Timer*> CTimers::m_timers;

Timer::Timer(int interval, int repeats, void(*callback)())
    : m_interval(interval), m_repeats(repeats), m_repeated(0), m_callback(callback)
{
    m_lastCall = std::chrono::steady_clock::now();
}

void Timer::Update()
{
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastCall).count();

    if (duration >= m_interval)
    {
        m_lastCall = now;
        m_callback();

        if (m_repeats > 0)
        {
            m_repeated++;
            if (m_repeated >= m_repeats)
            {
                Kill();
            }
        }
    }
}

bool Timer::IsFinished()
{
    return (m_repeats > 0 && m_repeated >= m_repeats);
}

void Timer::Reset()
{
    m_lastCall = std::chrono::steady_clock::now();
    m_repeated = 0;
}

void Timer::Kill()
{
    auto it = std::find(CTimers::m_timers.begin(), CTimers::m_timers.end(), this);
    if (it != CTimers::m_timers.end())
    {
        CTimers::m_timers.erase(it);
        delete this;
    }
}

void Timer::Trigger()
{
    m_callback();
}

void CTimers::Update()
{
    for (auto timer : m_timers)
    {
        timer->Update();
        if (timer->IsFinished())
        {
            timer->Kill();
        }
    }
}

void CTimers::CreateTimer(int interval, int repeats, void(*callback)())
{
    Timer* newTimer = new Timer(interval, repeats, callback);
    m_timers.push_back(newTimer);
}

void CTimers::KillTimer(void(*callback)())
{
    for (auto timer : m_timers)
    {
        if (timer->m_callback == callback)
        {
            timer->Kill();
        }
    }
}

void CTimers::InitAsync()
{
    while (true)
    {
		Update();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void CTimers::Init()
{
    std::thread(InitAsync).detach();
}