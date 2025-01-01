#include "pch.h"

#include "Application.h"

#include "Scene.h"

#include "Window/Window.h"

#include "Entities/Marker.h"
#include "Entities/Mesh.h"

#include "Util/Log.h"

Application* Application::s_instance = nullptr;

Application::Application() 
    : m_running(true), m_lastFrameTime(0.0f)
{
    assert(!s_instance && "Application already exists");
    s_instance = this;

    LOG_INIT();

    Window::create("Application", 800, 600);
    Window::setEventCallback(BIND_EVENT_FUNCTION(Application::onEvent));
    Scene::init();

    signal(SIGTERM, Application::handleSignal);
    signal(SIGINT, Application::handleSignal);
}

Application::~Application()
{
    Window::shutdown();

    LOG_SHUTDOWN();
}

int Application::run(int argc, char **argv)
{
    std::cout << "Starting Application" << std::endl;

    if (argc != 2) {
		std::cerr << "No robot model path specified." << std::endl;
		return 1;
	}

    for (int i = 0; i < 20; ++i)
        LOG_INFO << i;
    LOG_INFO_IMMEDIATELY << "lol";

    auto s = Timestamp().dateTimeStr();
    LOG_TRACE << s;
    s[3] = '1';
    LOG_TRACE << s;
    LOG_TRACE << Timestamp(s);
    
    // const std::string file = argv[1];
    // const auto parts = splitString(argv[1], "/");
    // if (parts.size() != 2 || aiIsExtensionSupported(parts[1].c_str()) == AI_FALSE) {
	// 	std::cerr << "Invalid input model file specified." << std::endl;
	// 	return 1;
	// }

    // m_scene = aiImportFile(file.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
    // if (!m_scene) {
    //     std::cerr << "Loading input model file failed." << std::endl;
	// 	return 1;
    // }

    std::shared_ptr<Entity> origin = Scene::createMarker("Origin");
    origin->scale({0.75f, 0.75f, 0.75f});

    // todo: change entity type
    std::shared_ptr<Entity> dragMarker = Scene::createMarker("DragMarker");
    // dragMarker->scale({5.0f, 5.0f, 5.0f});
    dragMarker->setVisible(false);

    if (!Scene::createRobot(argv[1])) {
        std::cerr << "Failed to create the robot." << std::endl;
		return 1;
    }

    while (m_running) {
        const float time = static_cast<float>(glfwGetTime())/1000.0f;
        const Timestep dt = time - m_lastFrameTime;
        m_lastFrameTime = time;

        onUpdate(dt);
    }

    return 0;
}

void Application::close()
{
    m_running = false;
}

void Application::onUpdate(const Timestep dt)
{
    Window::onUpdate();
    Scene::onUpdate(dt);
}

void Application::onEvent(Event& e)
{
    // std::cout << "Event: " << e.toString() << std::endl;
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
    std::cout << "Signal caught: " << signal << std::endl;
    WindowCloseEvent event;
    Application::s_instance->onWindowClose(event);
}