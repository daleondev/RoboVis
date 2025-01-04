#pragma once

#include "Entity.h"

#include "Renderer/VertexArray.h"

class Texture2D;

struct PlaneData
{
    std::array<glm::vec3, 4> vertices;
    std::array<std::array<uint16_t, 3>, 2> indices;
};

class Plane : public Entity {

public:
    Plane(const std::shared_ptr<Shader>& shader, const std::variant<std::shared_ptr<Texture2D>, glm::vec4>& material);
    virtual ~Plane();

    virtual void draw(const std::optional<Camera>& camera = {}) override;

    PlaneData getData() const;

private:   
    virtual void createBuffers() override;

    inline bool hasTexture() const { return m_material.index() == 0; }

    std::variant<std::shared_ptr<Texture2D>, glm::vec4> m_material;
    VertexArray m_vertexArray;

};
