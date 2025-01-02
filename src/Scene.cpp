#include "pch.h"

#include "Scene.h"

#include "Window/Input.h"

#include "Renderer/Renderer.h"

#include "Entities/Marker.h"
#include "Entities/Mesh.h"

#include "Util/geometry.h"

std::unordered_map<std::string, std::shared_ptr<Entity>> Scene::s_entities;
Robot Scene::s_robot;

void Scene::init()
{
    const auto r_cam_world = angleAxisF(M_PIf32/2 + M_PIf32/8, glm::vec3(1.0f, 0.0f, 0.0f)) * angleAxisF(-M_PIf32/4, glm::vec3(0.0f, 0.0f, 1.0f));
    const auto p_cam_world = glm::vec3(2.0f, 2.0f, 2.0f);

    glm::mat4 t_cam_world(1.0f);
    setMat4Rotation(t_cam_world, r_cam_world);
    setMat4Translation(t_cam_world, p_cam_world);

    CameraController::init(70.0f, 0.3f, 1000.0f, t_cam_world);
    Renderer::init();
}

bool Scene::createRobot(const std::string& sourceDir)
{
    return s_robot.setup(sourceDir);
}

std::shared_ptr<Mesh> Scene::createMesh(const std::string& name, const aiScene* source, const glm::mat4& t_mesh_world, const glm::mat4& initialTransformation)
{
    std::shared_ptr<Shader> shader;
    if (ShaderLibrary::exists("Mesh"))
        shader = ShaderLibrary::get("Mesh");
    else
        shader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/Mesh", "Mesh");

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(shader, source, t_mesh_world);
    mesh->setTransformation(initialTransformation);
    addEntity(name, mesh);
    return mesh;
}

std::shared_ptr<Marker> Scene::createMarker(const std::string& name, const glm::mat4& initialTransformation)
{
    std::shared_ptr<Shader> shader;
    if (ShaderLibrary::exists("Marker"))
        shader = ShaderLibrary::get("Marker");
    else
        shader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/Marker", "Marker");

    std::shared_ptr<Marker> marker = std::make_shared<Marker>(shader);
    marker->setTransformation(initialTransformation);
    addEntity(name, marker);
    return marker;
}

void Scene::addEntity(const std::string& name, const std::shared_ptr<Entity>& entity) 
{ 
    assert(s_entities.find(name) == s_entities.end() && "Entity already exists");
    s_entities.emplace(name, entity); 
}

std::shared_ptr<Entity> Scene::getEntity(const std::string& name)  
{ 
    assert(s_entities.find(name) != s_entities.end() && "Entity doesnt exist");
    return s_entities[name];
}

void Scene::deleteEntity(const std::string& name) 
{ 
    auto it = s_entities.find(name);
    assert(it != s_entities.end() && "Entity doesnt exist");
    s_entities.erase(it);
}

void Scene::onUpdate(const Timestep dt)
{   
    Renderer::clear({0.9f, 0.9f, 0.9f, 1.0f}); 
    CameraController::onUpdate(dt);

    s_robot.onUpdate(dt);

    for (const auto&[name, entity] : s_entities) {

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

bool Scene::onMouseLeave(MouseLeaveEvent& e)
{   
    CameraController::stopInteraction();

    return true;
}

bool Scene::onMouseMoved(MouseMovedEvent& e)
{   
    if (CameraController::isDragging()) {
        const auto pos = e.getPosition();
        CameraController::drag({pos.first, pos.second});
    }

    return true;
}

bool Scene::onMouseButtonPressed(MouseButtonPressedEvent& e)
{
    switch(e.getMouseButton()) {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            CameraController::startDraggingTrans(Input::GetMousePosition());
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            CameraController::startDraggingRot(Input::GetMousePosition());
            break;
        }
    }

    if (CameraController::isDragging() && entityExists("DragMarker")) {
        auto marker = getEntity("DragMarker");
        marker->setTranslation(CameraController::getDraggingPosition());
        marker->setVisible(true);
    }

    return true;
}

bool Scene::onMouseButtonReleased(MouseButtonReleasedEvent& e)
{
    switch(e.getMouseButton()) {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            CameraController::stopDraggingTrans();
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            CameraController::stopDraggingRot();
            if (entityExists("DragMarker"))
                getEntity("DragMarker")->setVisible(false);
            break;
        }
    }

    return true;
}

bool Scene::onMouseScrolled(MouseScrolledEvent& e)
{
    CameraController::zoom(e.getYOffset());

    return true;
}

bool Scene::onWindowResized(WindowResizeEvent& e)
{
    CameraController::onResize();

    return true;
}