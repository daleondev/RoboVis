#pragma once

// #define BIND_EVENT_FUNCTION(fn) std::bind(&fn, this, std::placeholders::_1)
#define BIND_EVENT_FUNCTION(fn) [this]<typename T>(T&& x) { return fn(std::forward<T>(x)); }

enum class EventType : uint8_t {
    None = 0,
    WindowClose, WindowResize,
    KeyPressed, KeyReleased, KeyTyped,
    MouseEnterEvent, MouseLeave, MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum class EventCategory : uint8_t {
    None            = 0,
    Window          = (1 << 0),
    Input           = (1 << 1),
    Keyboard        = (1 << 2),
    Mouse           = (1 << 3),
    MouseButton     = (1 << 4),    
};

#define EVENT_CLASS_TYPE(type)  static EventType getStaticType() { return EventType::type; } \
                            virtual EventType getEventType() const override { return getStaticType(); } \
                            virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual uint8_t getCategoryFlags() const override { return category; }

class Event {
public:
    Event() = default;
    virtual ~Event() = default;

    virtual EventType getEventType() const = 0;
    virtual const char* getName() const = 0;
    virtual uint8_t getCategoryFlags() const = 0;
    virtual inline std::string toString() const { return getName(); }

    inline bool isInCategory(EventCategory category) { return getCategoryFlags() & static_cast<uint8_t>(category); }

    inline bool isHandled() const { return m_handled; }
    inline void setHandled(bool handled) { m_handled = handled; }

private:
    bool m_handled = false;

};

typedef std::function<void(Event&)> EventCallbackFunction;

class EventDispatcher {
public:
    EventDispatcher(Event& event) : m_event(event) {}

    template<typename T, typename F>
    inline bool dispatch(const F& func)
    {
        if (m_event.getEventType() == T::getStaticType()) {
            m_event.setHandled(m_event.isHandled() | func(static_cast<T&>(m_event)));
            return true;
        }
        return false;
    }

private:
    Event& m_event;

};

// static std::ostream& operator<<(std::ostream& os, const Event& e)
// {
//     return os << e.toString();
// }
