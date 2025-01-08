#pragma once

#include "Entities/Entity.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

#include "Renderer/Texture.h"
#include "Renderer/FrameBuffer.h"

#include "Util/util.h"

class Entity;

class Scene
{
public:
    static void init();

    static Entity createEntity(const std::string& tag);
    static void destroyEntity(const Entity entity);

    static void update(const Timestep dt);

    // static std::shared_ptr<Robot> createRobot(const std::string& name, const std::filesystem::path& sourceDir, const glm::mat4& initialTransformation = glm::mat4(1.0f));
    // static std::shared_ptr<Mesh> createMesh(const std::string& name, const aiScene* source, const glm::mat4& t_mesh_world = glm::mat4(1.0f), const glm::mat4& initialTransformation = glm::mat4(1.0f));
    // static std::shared_ptr<Frame> createFrame(const std::string& name, const glm::mat4& initialTransformation = glm::mat4(1.0f));
    // static std::shared_ptr<Plane> createPlane(const std::string& name, const std::shared_ptr<Texture2D>& texture, const glm::mat4& initialTransformation = glm::mat4(1.0f));
    // static std::shared_ptr<Plane> createPlane(const std::string& name, const glm::vec4& color, const glm::mat4& initialTransformation = glm::mat4(1.0f));
    // static std::shared_ptr<Sphere> createSphere(const std::string& name, const glm::vec4& color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), const glm::mat4& initialTransformation = glm::mat4(1.0f));

    // static void addEntity(const std::string& name, const std::shared_ptr<EntityOld>& entity);
    // static std::shared_ptr<EntityOld> getEntity(const std::string& name);
    // static void deleteEntity(const std::string& name);

    // static void render(const Timestep dt);

    // inline static size_t entityExists(const std::string& name) { return s_entities.find(name) != s_entities.end(); }
    // inline static size_t numEntities() { return s_entities.size(); }
    // inline static std::unordered_map<std::string, std::shared_ptr<EntityOld>>& getEntities() { return s_entities; }

    inline static std::shared_ptr<FrameBuffer> getFrameBuffer() { return s_frameBuffer; }

    static bool onMouseLeave(MouseLeaveEvent& e);
    static bool onMouseMoved(MouseMovedEvent& e);
    static bool onMouseButtonPressed(MouseButtonPressedEvent& e);
    static bool onMouseButtonReleased(MouseButtonReleasedEvent& e);
    static bool onMouseScrolled(MouseScrolledEvent& e);
    static bool onMouseDropped(MouseDroppedEvent& e);
    static bool onWindowResized(WindowResizeEvent& e);

private:
    static std::shared_ptr<FrameBuffer> s_frameBuffer;

    static entt::registry s_registry;
    static std::unordered_map<UUID, Entity> s_entities;

    friend class Entity;

};