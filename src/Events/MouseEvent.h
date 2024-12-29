#pragma once

#include "Event.h"

class MouseMovedEvent : public Event {
public:
	MouseMovedEvent(const float x, const float y) : m_position(x, y) {}

	inline std::pair<float, float> getPosition() const { return m_position; }
	inline float getX() const { return m_position.first; }
	inline float getY() const { return m_position.second; }

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << m_position.first << ", " << m_position.second;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseMoved)
	EVENT_CLASS_CATEGORY(static_cast<uint8_t>(EventCategory::Mouse) | static_cast<uint8_t>(EventCategory::Input))

private:
	std::pair<float, float> m_position;

};

class MouseScrolledEvent : public Event {
public:
	MouseScrolledEvent(const float xOffset, const float yOffset) : m_offsets(xOffset, yOffset) {}

	inline std::pair<float, float> getOffsets() const { return m_offsets; }
	inline float getXOffset() const { return m_offsets.first; }
	inline float getYOffset() const { return m_offsets.second; }

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << m_offsets.first << ", " << m_offsets.second;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseScrolled)
	EVENT_CLASS_CATEGORY(static_cast<uint8_t>(EventCategory::Mouse) | static_cast<uint8_t>(EventCategory::Input))

private:
	std::pair<float, float> m_offsets;

};

class MouseButtonEvent : public Event {
public:
	inline int getMouseButton() const { return m_button; }

	EVENT_CLASS_CATEGORY(static_cast<uint8_t>(EventCategory::Mouse) | static_cast<uint8_t>(EventCategory::Input) | static_cast<uint8_t>(EventCategory::MouseButton))

protected:
	MouseButtonEvent(const int button) : m_button(button) {}

	int m_button;

};

class MouseButtonPressedEvent : public MouseButtonEvent {
public:
	MouseButtonPressedEvent(const int button) : MouseButtonEvent(button) {}

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonPressed)

};

class MouseButtonReleasedEvent : public MouseButtonEvent {
public:
	MouseButtonReleasedEvent(const int button) : MouseButtonEvent(button) {}

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << m_button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonReleased)

};