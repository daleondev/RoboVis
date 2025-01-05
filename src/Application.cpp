#include "pch.h"

#include "Application.h"

#include "Scene.h"

#include "Window/Window.h"

#include "ImGui/ImGuiLayer.h"

#include "Entities/Frame.h"
#include "Entities/Mesh.h"

#include "Util/Log.h"

Application* Application::s_instance = nullptr;

Application::Application() 
    : m_running(true), m_lastFrameTime(0.0f)
{
    assert(!s_instance && "Application already exists");
    s_instance = this;

    LOG_INIT();

    Window::create("Application", 1280, 720);
    Window::setEventCallback(BIND_EVENT_FUNCTION(Application::onEvent));
    ImGuiLayer::init();
    Scene::init();
    
    signal(SIGTERM, Application::handleSignal);
    signal(SIGINT, Application::handleSignal);
}

Application::~Application()
{
    ImGuiLayer::shutdown();
    Window::shutdown();

    LOG_SHUTDOWN();
}

int Application::run(int argc, char **argv)
{
    LOG_INFO << "Starting Application";

    if (argc != 2) {
		LOG_FATAL << "No robot model path specified.";
		return 1;
	}

    if (!Scene::createRobot(argv[1])) {
        LOG_FATAL << "Failed to create the robot.";
		return 1;
    }

    while (m_running) {
        const float time = static_cast<float>(glfwGetTime())/1000.0f;
        const Timestep dt = time - m_lastFrameTime;
        m_lastFrameTime = time;

        update(dt);
    }

    return 0;
}

void Application::close()
{
    LOG_WARN << "Stopping Application";
    m_running = false;
}

void Application::update(const Timestep dt)
{
    Window::update();

    Scene::render(dt);
    ImGuiLayer::render(dt);
}

void Application::onEvent(Event& e)
{
    ImGuiLayer::onEvent(e);
    if (e.isHandled())
        return;

    EventDispatcher dispatcher(e);
    
    // Window-Event
    if (e.getCategoryFlags() & static_cast<uint8_t>(EventCategory::Window)) {
        dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(onWindowClose));
        dispatcher.dispatch<WindowResizeEvent>(Scene::onWindowResized);
    }
    // Mouse-Event
    else if (e.getCategoryFlags() & static_cast<uint8_t>(EventCategory::Mouse)) {
        dispatcher.dispatch<MouseLeaveEvent>(Scene::onMouseLeave);
        dispatcher.dispatch<MouseMovedEvent>(Scene::onMouseMoved);
        dispatcher.dispatch<MouseButtonPressedEvent>(Scene::onMouseButtonPressed);
        dispatcher.dispatch<MouseButtonReleasedEvent>(Scene::onMouseButtonReleased);    
        dispatcher.dispatch<MouseScrolledEvent>(Scene::onMouseScrolled);
    }
}

bool Application::onWindowClose(WindowCloseEvent& e)
{   
    m_running = false;
    return true;
}

void Application::handleSignal(int signal)
{
    // LOG_WARN << "Signal caught: " << signal;    
    WindowCloseEvent event;
    Application::s_instance->onWindowClose(event);
}