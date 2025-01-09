#pragma once

#include "pch.h"

#include "Util/util.h"
#include "Util/geometry.h"

struct IdComponent
{
    UUID id;

    IdComponent() : id{uuid()} {}
    IdComponent(const IdComponent&) = default;
};

struct TagComponent
{
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : tag(tag) {}
};

struct TransformComponent
{
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;

    TransformComponent() : translation(0.0f), rotation(0.0f), scale(1.0f) {};
    TransformComponent(const TransformComponent&) = default;

    void set(const glm::mat4& transformation)
    {
        translation = getMat4Translation(transformation);

        const auto axisX = getMat4AxisX(transformation);
        const auto axisY = getMat4AxisY(transformation);
        const auto axisZ = getMat4AxisZ(transformation);
        scale.x = glm::length(axisX);
        scale.y = glm::length(axisY);
        scale.z = glm::length(axisZ);

        const auto r = getMat4Rotation(transformation);
        rotation = toEulerZYX(r);
    }

    glm::mat4 get() const
    {
        return glm::translate(glm::mat4(1.0f), translation)
            * glm::mat4(eulerZYX(rotation))
            * glm::scale(glm::mat4(1.0f), scale);
    }
};

class Texture2D;

struct PlaneRendererComponent
{
    std::variant<glm::vec4, std::shared_ptr<Texture2D>> m_material;

    PlaneRendererComponent() : m_material(glm::vec4(1.0f)) {}
    PlaneRendererComponent(const PlaneRendererComponent&) = default;
    PlaneRendererComponent(const glm::vec4& color) : m_material(color) {}
    PlaneRendererComponent(const std::shared_ptr<Texture2D>& texture) : m_material(texture) {}

    inline bool hasTexture() const { return m_material.index() == 1; }
};

struct FrameRendererComponent
{
    int test;

    FrameRendererComponent() = default;
    FrameRendererComponent(const FrameRendererComponent&) = default;
};