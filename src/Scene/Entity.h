#pragma once

#include "Scene.h"

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
        return Scene::s_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    T& addOrReplaceComponent(Args&&... args)
    {
        return Scene::s_registry.emplace_or_replace<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    T& addComponentIfNotExists(Args&&... args)
    {
        return hasComponent<T>() ? getComponent<T>() : Scene::s_registry.emplace_or_replace<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T>
    T& getComponent() const
    {
        assert(hasComponent<T>() && "Entity does not have component!");
        return Scene::s_registry.get<T>(m_handle);
    }

    template<typename T>
    inline bool hasComponent() const { return Scene::s_registry.all_of<T>(m_handle); }

    template<typename T>
    void RemoveComponent()
    {
        assert(hasComponent<T>() && "Entity does not have component!");
        Scene::s_registry.remove<T>(m_handle);
    }

    UUID getId() const;
    std::string getTag() const;

    inline operator bool() const { return m_handle != entt::null; }
    inline operator entt::entity() const { return m_handle; }
    inline operator uint32_t() const { return static_cast<uint32_t>(m_handle); }

    inline bool operator==(const Entity other) const { return m_handle == other.m_handle; }
    inline bool operator!=(const Entity other) const { return m_handle != other.m_handle; }
    inline bool operator<(const Entity other) const { return m_handle < other.m_handle; }
    inline bool operator>(const Entity other) const { return m_handle > other.m_handle; }
    inline bool operator<=(const Entity other) const { return m_handle <= other.m_handle; }
    inline bool operator>=(const Entity other) const { return m_handle >= other.m_handle; }

private:
    entt::entity m_handle;

};

inline static const Entity EntityNull{static_cast<entt::entity>(entt::null)};