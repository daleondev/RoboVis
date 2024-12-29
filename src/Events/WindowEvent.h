#pragma once

#include "Event.h"

class WindowResizeEvent : public Event {
public:
    WindowResizeEvent(const uint16_t width, const uint16_t height) : m_size(width, height) {}

    inline std::pair<uint16_t, uint16_t> getSize() const { return m_size; }
    inline uint16_t getWidth() const { return m_size.first; }
    inline uint16_t getHeight() const { return m_size.second; }        

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << m_size.first << ", " << m_size.second;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY((uint8_t)EventCategory::Window)

private:
    std::pair<uint16_t, uint16_t> m_size;

};

class WindowCloseEvent : public Event
{
public:
    WindowCloseEvent() = default;

    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY((uint8_t)EventCategory::Window)
};
