#pragma once

#include "Event.h"

class KeyEvent : public Event {
public:
	inline int getKeyCode() const { return m_keyCode; }

	EVENT_CLASS_CATEGORY(static_cast<uint8_t>(EventCategory::Keyboard) | static_cast<uint8_t>(EventCategory::Input))

protected:
	KeyEvent(const int keyCode) : m_keyCode(keyCode) {}

	int m_keyCode;

};

class KeyPressedEvent : public KeyEvent {
public:
	KeyPressedEvent(const int keyCode, const uint16_t repeatCount) : KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

	inline uint16_t GetRepeatCount() const { return m_RepeatCount; }

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_keyCode << " (" << m_RepeatCount << " repeats)";
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyPressed)

private:
	uint16_t m_RepeatCount;

};

class KeyReleasedEvent : public KeyEvent {
public:
	KeyReleasedEvent(const int keyCode) : KeyEvent(keyCode) {}

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << m_keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyReleased)

};

class KeyTypedEvent : public KeyEvent {
public:
	KeyTypedEvent(const int keycode) : KeyEvent(keycode) {}

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << m_keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyTyped)

};