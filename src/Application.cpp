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

    CameraController::init(53.0f, 0.3f, 1000.0f, glm::translate(glm::mat4(1.0f), {0, 20, -100}));
    Renderer::init();
}

Application::~Application()
{
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

    const aiScene* scene = aiImportFile(file.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
    if (!scene) {
        std::cerr << "Loading input model file failed." << std::endl;
		return 1;
    }

    ShaderLibrary shaderLib;
    auto markerShader = shaderLib.load("/home/david/Schreibtisch/RoboVis/src/Shaders/Marker");
    auto meshShader = shaderLib.load("/home/david/Schreibtisch/RoboVis/src/Shaders/Mesh");

    std::shared_ptr<Marker> marker = std::make_shared<Marker>(markerShader);
    marker->create();
    marker->rotate(M_1_PI, {0.2, 0.2, 0});
    marker->scale({0.4, 0.4, 0.4});

    std::vector<std::shared_ptr<Mesh>> meshes(scene->mNumMeshes);
    for (size_t i = 0; i < meshes.size(); ++i) {
        if (scene->mNumMaterials > 0 && scene->mMeshes[i]->mMaterialIndex < scene->mNumMaterials)
            meshes[i] = std::make_shared<Mesh>(meshShader, scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]);
        else {
            meshes[i] = std::make_shared<Mesh>(meshShader, scene->mMeshes[i], nullptr);
        }
        meshes[i]->create();
        // meshes[i]->rotate(M_PI_4, {0.0, 1.0, 0.0});
    }
    aiReleaseImport(scene);

    m_entities.push_back(marker);
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
    dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::onWindowClose));
}

bool Application::onWindowClose(WindowCloseEvent& e)
{   
    std::cout << "Closing Window" << std::endl;
    m_running = false;
    return true;
}

void Application::handleSignal(int signal)
{
    std::cout << "Signal caught: " << signal << std::endl;
    WindowCloseEvent event;
    Application::s_instance->onWindowClose(event);
}