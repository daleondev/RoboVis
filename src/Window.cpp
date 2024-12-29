#include "Window.h"

#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

GLFWwindow* Window::s_window;
Window::WindowData Window::s_data;
bool Window::s_initialized = false;

void Window::create(const std::string& title, const uint16_t width, const uint16_t height)
{
    s_data.title = title;
    s_data.width = width;
    s_data.height = height;
    s_data.vSync = false;

    if (!s_initialized)
        init();
}

void Window::shutdown()
{
    glfwDestroyWindow(s_window);
    glfwTerminate();
}

void Window::onUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(s_window);
}

void Window::setVSync(const bool enabled)
{
    (s_data.vSync = enabled) ? glfwSwapInterval(1) : glfwSwapInterval(0);
}

bool Window::getVSync()
{
    return s_data.vSync;
}

void Window::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    s_window = glfwCreateWindow(s_data.width, s_data.height, s_data.title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(s_window, &s_data);
    glfwMakeContextCurrent(s_window);
    setVSync(true);

    glfwSetWindowSizeCallback(s_window, [](GLFWwindow* window, int width, int height) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(s_window));
        data.width = width;
        data.height = height;

        WindowResizeEvent event(width, height);
        data.eventCallback(event);
    });

    glfwSetWindowCloseCallback(s_window, [](GLFWwindow* window) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(s_window));

        WindowCloseEvent event;
        data.eventCallback(event);
    });

    glfwSetKeyCallback(s_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(s_window));
        
        switch(action)
        {
            case GLFW_PRESS:
            {
                KeyPressedEvent event(key, 0);
                data.eventCallback(event);
            }
            case GLFW_RELEASE:
            {
                KeyReleasedEvent event(key);
                data.eventCallback(event);
            }
            case GLFW_REPEAT:
            {
                KeyPressedEvent event(key, 1);
                data.eventCallback(event);
            }
        }
    });

    glfwSetMouseButtonCallback(s_window, [](GLFWwindow* window, int button, int action, int mods) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(s_window));
        
        switch(action)
        {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(button);
                data.eventCallback(event);
            }
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(button);
                data.eventCallback(event);
            }
        }
    });

    glfwSetScrollCallback(s_window, [](GLFWwindow* window, double xOffset, double yOffset) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(s_window));
        
        MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
        data.eventCallback(event);
    });

    glfwSetCursorPosCallback(s_window, [](GLFWwindow* window, double xPos, double yPos) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(s_window));
        
        MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
        data.eventCallback(event);
    });

    s_initialized = true;
}