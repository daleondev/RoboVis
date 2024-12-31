#pragma once

#include "Window.h"

class Input
{
public:
    static bool isKeyPressed(const int key) 
    { 
		auto state = glfwGetKey(Window::getNativeWindow(), key);
		return state == GLFW_PRESS;
    }

    static bool isMousePressed(const int button) 
    { 
		auto state = glfwGetMouseButton(Window::getNativeWindow(), button);
		return state == GLFW_PRESS;
    }

    static glm::vec2 GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(Window::getNativeWindow(), &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}
};