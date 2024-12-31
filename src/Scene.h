#pragma once

#include "Entities/Entity.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

class Scene
{
public:
    static void init();

    static std::shared_ptr<Entity> createMesh(const std::string& name, const aiScene* source, const glm::mat4& initialTransformation = glm::mat4(1.0f));
    static std::shared_ptr<Entity> createMarker(const std::string& name, const glm::mat4& initialTransformation = glm::mat4(1.0f));

    static void addEntity(const std::string& name, const std::shared_ptr<Entity>& entity);
    static std::shared_ptr<Entity> getEntity(const std::string& name);
    static void deleteEntity(const std::string& name);

    inline static size_t entityExists(const std::string& name) { return s_entities.find(name) != s_entities.end(); }
    inline static size_t numEntities() { return s_entities.size(); }
    inline static std::unordered_map<std::string, std::shared_ptr<Entity>>& getEntities() { return s_entities; }

    static void onUpdate(const Timestep dt);
    static bool onMouseLeave(MouseLeaveEvent& e);
    static bool onMouseMoved(MouseMovedEvent& e);
    static bool onMouseButtonPressed(MouseButtonPressedEvent& e);
    static bool onMouseButtonReleased(MouseButtonReleasedEvent& e);
    static bool onMouseScrolled(MouseScrolledEvent& e);
    static bool onWindowResized(WindowResizeEvent& e);

private:
    static std::unordered_map<std::string, std::shared_ptr<Entity>> s_entities;

};