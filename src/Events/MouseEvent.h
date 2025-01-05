#pragma once

#include "Event.h"

class MouseEnterEvent : public Event
{
public:
	MouseEnterEvent() = default;

	std::string toString() const override
	{
		return "MouseEnterEvent";
	}

	EVENT_CLASS_TYPE(MouseEnterEvent)
	EVENT_CLASS_CATEGORY(static_cast<uint8_t>(EventCategory::Mouse))
};

class MouseLeaveEvent : public Event
{
public:
	MouseLeaveEvent() = default;

	std::string toString() const override
	{
		return "MouseLeaveEvent";
	}

	EVENT_CLASS_TYPE(MouseLeave)
	EVENT_CLASS_CATEGORY(static_cast<uint8_t>(EventCategory::Mouse))
};

class MouseMovedEvent : public Event {
public:
	MouseMovedEvent(const float x, const float y) : m_position(x, y) {}

	inline glm::vec2 getPosition() const { return m_position; }
	inline float getX() const { return m_position.x; }
	inline float getY() const { return m_position.y; }

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << m_position.x << ", " << m_position.y;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseMoved)
	EVENT_CLASS_CATEGORY(static_cast<uint8_t>(EventCategory::Mouse) | static_cast<uint8_t>(EventCategory::Input))

private:
	glm::vec2 m_position;

};

class MouseScrolledEvent : public Event {
public:
	MouseScrolledEvent(const float xOffset, const float yOffset) : m_offsets(xOffset, yOffset) {}

	inline glm::vec2 getOffsets() const { return m_offsets; }
	inline float getXOffset() const { return m_offsets.x; }
	inline float getYOffset() const { return m_offsets.y; }

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << m_offsets.x << ", " << m_offsets.y;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseScrolled)
	EVENT_CLASS_CATEGORY(static_cast<uint8_t>(EventCategory::Mouse) | static_cast<uint8_t>(EventCategory::Input))

private:
	glm::vec2 m_offsets;

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

class MouseDroppedEvent : public Event
{
public:
	MouseDroppedEvent(const std::vector<std::string>& paths) : m_paths(paths) {}

	inline std::vector<std::string> getPaths() const { return m_paths; }
	inline size_t getNumPaths() const { return m_paths.size(); }
	inline std::string getPath(const size_t idx) const { return m_paths[idx]; }

	inline std::vector<std::string>::const_iterator cbegin() const { return m_paths.cbegin(); }
	inline std::vector<std::string>::const_iterator cend() const { return m_paths.cend(); }
	inline std::vector<std::string>::iterator begin() { return m_paths.begin(); }
	inline std::vector<std::string>::iterator end() { return m_paths.end(); }

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "MouseDropEvent";
		for (size_t i = 0; i < m_paths.size(); ++i)
			ss << "\nPath_" << i << ": " << m_paths[i];
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseDropped)
	EVENT_CLASS_CATEGORY(static_cast<uint8_t>(EventCategory::Mouse))

private:
	std::vector<std::string> m_paths;

};