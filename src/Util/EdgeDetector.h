#pragma once

template <typename T>
class EdgeDetector
{
public:
    EdgeDetector() = default;
    ~EdgeDetector() = default;

    EdgeDetector& operator()()
    {
        if (std::is_same<T, bool>::value) {
            m_rising = m_val && !m_prev;
            m_falling = !m_val && m_prev;
            m_edge = m_rising || m_falling;
        }
        else {
            m_rising = false;
            m_falling = false;
            m_edge = m_val != m_prev;
        }
        m_prev = m_val;
        return *this;
    }

    inline T& val() { return m_val; };

    inline bool edge() const { return m_edge; };
    inline bool rising() const { return m_rising; };
    inline bool falling() const { return m_falling; };

private:
    T m_val;
    T m_prev;

    bool m_edge;
    bool m_rising;
    bool m_falling;

};