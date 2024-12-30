#include "Application.h"

#include "Window.h"
#include "Renderer.h"
#include "Scene.h"

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

    // auto markerShader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/Marker");
    // auto meshShader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/Mesh");

    // Scene::createMarker("Marker1");
    Scene::createMesh("Mesh1", m_scene);

    // std::shared_ptr<Marker> marker = std::make_shared<Marker>(markerShader);
    // marker->create();
    // // marker->scale({10, 10, 10});

    // std::shared_ptr<Marker> marker2 = std::make_shared<Marker>(markerShader);
    // marker2->create();
    // marker2->translate({0.0, 0.0, 5});
    // marker2->scale({0.2, 0.2, 0.2});

    // std::vector<std::shared_ptr<Mesh>> meshes(m_scene->mNumMeshes);
    // for (size_t i = 0; i < meshes.size(); ++i) {
    //     if (m_scene->mNumMaterials > 0 && m_scene->mMeshes[i]->mMaterialIndex < m_scene->mNumMaterials)
    //         meshes[i] = std::make_shared<Mesh>(meshShader, m_scene->mMeshes[i], m_scene->mMaterials[m_scene->mMeshes[i]->mMaterialIndex]);
    //     else {
    //         meshes[i] = std::make_shared<Mesh>(meshShader, m_scene->mMeshes[i], nullptr);
    //     }
    //     meshes[i]->create();
    //     // meshes[i]->rotate(-M_PI_2, {1.0, 0.0, 0.0});
    //     // meshes[i]->rotate(M_PI_4, {0.0, 1.0, 0.0});
    // }

    // m_entities.push_back(marker);
    // m_entities.push_back(marker2);
    // for (auto& mesh : meshes) {
    //     m_entities.push_back(mesh);
    // }

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