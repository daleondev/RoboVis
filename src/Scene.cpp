#include "Scene.h"

#include "Input.h"

#include "Entities/Marker.h"
#include "Entities/Mesh.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include "geometry.h"

std::unordered_map<std::string, std::shared_ptr<Entity>> Scene::s_entities;

void Scene::init()
{
    glm::mat4 t(1.0f);
    t = glm::toMat4(glm::angleAxis(M_PIf32, glm::vec3(0.0f, 1.0f, 0.0f)));
    t[2][3] = 20.0f;
    CameraController::init(70.0f, 0.3f, 1000.0f, t);
    Renderer::init();
}

std::shared_ptr<Entity> Scene::createMesh(const std::string& name, const aiScene* source, const glm::mat4& initialTransformation)
{
    std::shared_ptr<Shader> shader;
    if (ShaderLibrary::exists("Mesh"))
        shader = ShaderLibrary::get("Mesh");
    else
        shader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/Mesh", "Mesh");

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(shader, source);
    mesh->setTransformation(initialTransformation);
    addEntity(name, mesh);
    return mesh;
}

std::shared_ptr<Entity> Scene::createMarker(const std::string& name, const glm::mat4& initialTransformation)
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
    // std::cout << "onMouseLeave" << std::endl;
    CameraController::stopInteraction();

    return true;
}

bool Scene::onMouseMoved(MouseMovedEvent& e)
{   
    // std::cout << "onMouseMoved " << e.toString() << std::endl;
    if (CameraController::isDragging()) {
        const auto pos = e.getPosition();
        CameraController::drag({pos.first, pos.second});
    }

    return true;
}

bool Scene::onMouseButtonPressed(MouseButtonPressedEvent& e)
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
            CameraController::startDraggingRot(Input::GetMousePosition());
            break;
        }
    }

    if (CameraController::isDragging() && entityExists("DragMarker"))
        if (auto pos = CameraController::getDraggingPosition(); pos) {
            auto marker = getEntity("DragMarker");
            marker->setTranslation(*pos);
            marker->setVisible(true);
            std::cout << pos->x << ", "  << pos->y << ", " << pos->z << "\n";
        }

    return true;
}

bool Scene::onMouseButtonReleased(MouseButtonReleasedEvent& e)
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
            if (entityExists("DragMarker"))
                getEntity("DragMarker")->setVisible(false);
            break;
        }
    }

    return true;
}

bool Scene::onMouseScrolled(MouseScrolledEvent& e)
{
    // std::cout << "onMouseScrolled" << std::endl;
    CameraController::zoom(e.getYOffset());

    return true;
}

bool Scene::onWindowResized(WindowResizeEvent& e)
{
    // std::cout << "onWindowResized" << e.toString() << std::endl;
    CameraController::onResize();

    return true;
}