#include "Application.h"

#include "Window.h"
#include "Renderer.h"
#include "Camera.h"
#include "Input.h"

#include "Entities/Marker.h"
#include "Entities/Mesh.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cassert>
#include <csignal>
#include <iostream>

Application* Application::s_instance = nullptr;

static std::vector<std::string> splitString(std::string str, const char* delims)
{
	std::vector<std::string> parts;

	size_t prev = 0, pos;
	while ((pos = str.find_first_of(delims, prev)) != std::string::npos) {
		if (pos > prev)
			parts.push_back(str.substr(prev, pos - prev));
		prev = pos + 1;
	}
	if (prev < str.size())
		parts.push_back(str.substr(prev, std::string::npos));

	return parts;
}

Application::Application() 
    : m_running(true), m_lastFrameTime(0.0f)
{
    assert(!s_instance && "Application already exists");
    s_instance = this;

    signal(SIGTERM, Application::handleSignal);
    signal(SIGINT, Application::handleSignal);

    Window::create("Application", 800, 600);
    Window::setEventCallback(BIND_EVENT_FUNCTION(Application::onEvent));

    CameraController::init(70.0f, 0.3f, 1000.0f, glm::translate(glm::mat4(1.0f), {0, 0, -20}));
    Renderer::init();
}

Application::~Application()
{
    aiReleaseImport(m_scene);
    Window::shutdown();
}

int Application::run(int argc, char **argv)
{
    std::cout << "Starting Application" << std::endl;

    if (argc != 2) {
		std::cerr << "No input model file specified." << std::endl;
		return 1;
	}

    const std::string file = argv[1];
    const auto parts = splitString(argv[1], ".");
    if (parts.size() != 2 || aiIsExtensionSupported(parts[1].c_str()) == AI_FALSE) {
		std::cerr << "Invalid input model file specified." << std::endl;
		return 1;
	}

    m_scene = aiImportFile(file.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
    if (!m_scene) {
        std::cerr << "Loading input model file failed." << std::endl;
		return 1;
    }

    ShaderLibrary shaderLib;
    auto markerShader = shaderLib.load("/home/david/Schreibtisch/RoboVis/src/Shaders/Marker");
    auto meshShader = shaderLib.load("/home/david/Schreibtisch/RoboVis/src/Shaders/Mesh");

    std::shared_ptr<Marker> marker = std::make_shared<Marker>(markerShader);
    marker->create();
    // marker->scale({10, 10, 10});

    std::shared_ptr<Marker> marker2 = std::make_shared<Marker>(markerShader);
    marker2->create();
    marker2->translate({0.0, 0.0, 5});
    marker2->scale({0.2, 0.2, 0.2});

    std::vector<std::shared_ptr<Mesh>> meshes(m_scene->mNumMeshes);
    for (size_t i = 0; i < meshes.size(); ++i) {
        if (m_scene->mNumMaterials > 0 && m_scene->mMeshes[i]->mMaterialIndex < m_scene->mNumMaterials)
            meshes[i] = std::make_shared<Mesh>(meshShader, m_scene->mMeshes[i], m_scene->mMaterials[m_scene->mMeshes[i]->mMaterialIndex]);
        else {
            meshes[i] = std::make_shared<Mesh>(meshShader, m_scene->mMeshes[i], nullptr);
        }
        meshes[i]->create();
        // meshes[i]->rotate(-M_PI_2, {1.0, 0.0, 0.0});
        // meshes[i]->rotate(M_PI_4, {0.0, 1.0, 0.0});
    }

    m_entities.push_back(marker);
    m_entities.push_back(marker2);
    for (auto& mesh : meshes) {
        m_entities.push_back(mesh);
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
    CameraController::onUpdate(dt);

    auto pos = Input::GetMousePosition();

    Renderer::clear({0.9f, 0.9f, 0.9f, 1.0f});
    for (const auto& entity : m_entities) {

        if (Input::isKeyPressed(GLFW_KEY_LEFT))
            entity->rotate(-500*dt, {0.0f, 1.0f, 0.0f});

        else if(Input::isKeyPressed(GLFW_KEY_RIGHT))
            entity->rotate(500*dt, {0.0f, 1.0f, 0.0f});

        if (Input::isKeyPressed(GLFW_KEY_UP))
            entity->rotate(-500*dt, {1.0f, 0.0f, 0.0f});

        else if(Input::isKeyPressed(GLFW_KEY_DOWN))
            entity->rotate(500*dt, {1.0f, 0.0f, 0.0f});

        entity->draw(CameraController::getCamera());
    }    
}

void Application::onEvent(Event& e)
{
    // std::cout << "Event: " << e.toString() << std::endl;

    EventDispatcher dispatcher(e);

    // Window-Event
    if (e.getCategoryFlags() & static_cast<uint8_t>(EventCategory::Window)) {
        dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(onWindowClose));
    }
    // Mouse-Event
    else if (e.getCategoryFlags() & static_cast<uint8_t>(EventCategory::Mouse)) {
        dispatcher.dispatch<MouseLeaveEvent>(BIND_EVENT_FUNCTION(onMouseLeave));
        dispatcher.dispatch<MouseMovedEvent>(BIND_EVENT_FUNCTION(onMouseMoved));
        dispatcher.dispatch<MouseButtonPressedEvent>(BIND_EVENT_FUNCTION(onMouseButtonPressed));
        dispatcher.dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FUNCTION(onMouseButtonReleased));    
        dispatcher.dispatch<MouseScrolledEvent>(BIND_EVENT_FUNCTION(onMouseScrolled));
    }
}

bool Application::onWindowClose(WindowCloseEvent& e)
{   
    // std::cout << "Closing Window" << std::endl;
    m_running = false;

    return true;
}

bool Application::onMouseLeave(MouseLeaveEvent& e)
{   
    // std::cout << "onMouseLeave" << std::endl;
    CameraController::stopInteraction();

    return true;
}

bool Application::onMouseMoved(MouseMovedEvent& e)
{   
    // std::cout << "onMouseMoved " << e.toString() << std::endl;
    if (CameraController::isDragging()) {
        const auto pos = e.getPosition();
        CameraController::drag({pos.first, pos.second});
    }

    return true;
}

bool Application::onMouseButtonPressed(MouseButtonPressedEvent& e)
{
    // std::cout << "onMouseButtonPressed" << std::endl;
    switch(e.getMouseButton()) {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            CameraController::startDraggingTrans(Input::GetMousePosition());
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            CameraController::startDraggingRot(Input::GetMousePosition(), m_scene);
            break;
        }
    }

    if (CameraController::isDragging())
        if (auto pos = CameraController::getDraggingPosition(); pos) {
            glm::mat4 t(1.0f);
            t[0][3] = pos->x;
            t[1][3] = pos->y;
            t[2][3] = pos->z;
            std::cout << pos->x << ", " << pos->y << ", " << pos->z << "\n";
            // m_entities[0]->scale({0.1f, 0.1f, 0.1f});
            m_entities[0]->setTranslation(*pos);
            // m_entities[0]->scale({10.0f, 10.0f, 0.1f});
        }

    return true;
}

bool Application::onMouseButtonReleased(MouseButtonReleasedEvent& e)
{
    // std::cout << "onMouseButtonReleased" << std::endl;
    switch(e.getMouseButton()) {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            CameraController::stopDraggingTrans();
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            CameraController::stopDraggingRot();
            break;
        }
    }

    return true;
}

bool Application::onMouseScrolled(MouseScrolledEvent& e)
{
    // std::cout << "onMouseScrolled" << std::endl;
    CameraController::zoom(e.getYOffset());

    return true;
}

void Application::handleSignal(int signal)
{
    std::cout << "Signal caught: " << signal << std::endl;
    WindowCloseEvent event;
    Application::s_instance->onWindowClose(event);
}