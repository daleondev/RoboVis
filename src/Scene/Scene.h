#pragma once

#include "Entities/Entity.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

#include "Renderer/Texture.h"
#include "Renderer/FrameBuffer.h"

#include "Util/util.h"
#include "Util/Timestep.h"

class Entity;

class Scene
{
public:
    static void init();

    static Entity createEntity(const std::string& tag);
    static void destroyEntity(const Entity entity);

    static Entity loadMesh();

    static void update(const Timestep dt);

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