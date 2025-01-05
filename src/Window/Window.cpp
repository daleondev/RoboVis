#include "pch.h"

#include "Window.h"

#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Util/Log.h"

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

void Window::update()
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    s_window = glfwCreateWindow(s_data.width, s_data.height, s_data.title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(s_window, &s_data);
    glfwMakeContextCurrent(s_window);
    setVSync(true);

#ifdef __cpp_lib_format
    LOG_INFO << "OpenGL:";
    FMT_INFO("\tOpenGL Vendor: {}", (char*)glGetString(GL_VENDOR));
    FMT_INFO("\tOpenGL Renderer: {}", (char*)glGetString(GL_RENDERER));
    FMT_INFO("\tOpenGL Version: {}", (char*)glGetString(GL_VERSION));
#endif

    glfwSetWindowSizeCallback(s_window, [](GLFWwindow* window, int width, int height) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        data.width = width;
        data.height = height;

        WindowResizeEvent event(width, height);
        data.eventCallback(event);
    });

    glfwSetWindowCloseCallback(s_window, [](GLFWwindow* window) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

        WindowCloseEvent event;
        data.eventCallback(event);
    });

    glfwSetKeyCallback(s_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        
        switch(action)
        {
            case GLFW_PRESS:
            {
                KeyPressedEvent event(key, 0);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                KeyReleasedEvent event(key);
                data.eventCallback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                KeyPressedEvent event(key, 1);
                data.eventCallback(event);
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(s_window, [](GLFWwindow* window, int button, int action, int mods) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        
        switch(action)
        {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(button);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(button);
                data.eventCallback(event);
                break;
            }
        }
    });

    glfwSetScrollCallback(s_window, [](GLFWwindow* window, double xOffset, double yOffset) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        
        MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
        data.eventCallback(event);
    });

    glfwSetCursorPosCallback(s_window, [](GLFWwindow* window, double xPos, double yPos) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        
        MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
        data.eventCallback(event);
    });

    glfwSetCursorEnterCallback(s_window, [](GLFWwindow* window, int entered) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        
        if (entered) {
            MouseEnterEvent event;
            data.eventCallback(event);
        }
        else {
            MouseLeaveEvent event;
            data.eventCallback(event);
        }
    });

    glfwSetDropCallback(s_window, [](GLFWwindow* window, int pathCount, const char* cpaths[]) -> void {
        auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        
        std::vector<std::string> paths(pathCount);
        for (int i = 0; i < pathCount; ++i) {
            paths[i] = cpaths[i];
        }

        MouseDroppedEvent event(paths);
        data.eventCallback(event);
    });

    s_initialized = true;
}