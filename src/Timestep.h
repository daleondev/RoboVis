#pragma once

class Timestep
{
public:
    Timestep(const float time = 0.0f)
        : m_time(time)
    {
    }

    inline float getSeconds() const { return m_time; }
    inline float getMilliSeconds() const { return m_time * 1000.0f; }

    inline operator float() const { return m_time; }

private:
    float m_time;
};