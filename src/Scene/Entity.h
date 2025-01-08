#pragma once

#include "Scene.h"
#include "Components.h"

#include <entt/entt.hpp>

class Entity
{
public:
    Entity() = delete;
    Entity(entt::entity handle) : m_handle{handle} {}
    Entity(const Entity&) = default;
    ~Entity() = default;

    template<typename T, typename... Args>
    T& addComponent(Args&&... args)
    {
        assert(!hasComponent<T>() && "Entity already has component!");
        T& component = Scene::s_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
        return component;
    }

    template<typename T>
    T& getComponent() const
    {
        assert(hasComponent<T>() && "Entity does not have component!");
        return Scene::s_registry.get<T>(m_handle);
    }

    template<typename T>
    bool hasComponent() const { return Scene::s_registry.all_of<T>(m_handle); }

    inline UUID getId() const { return getComponent<IdComponent>().id; }
    inline std::string getTag() const { return getComponent<TagComponent>().tag; }

    inline operator entt::entity() const { return m_handle; }
    inline operator uint32_t() const { return static_cast<uint32_t>(m_handle); }

private:
    entt::entity m_handle;

};