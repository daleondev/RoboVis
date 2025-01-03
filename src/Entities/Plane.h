#pragma once

#include "Entity.h"

#include "Renderer/VertexArray.h"

class Texture2D;

class Plane : public Entity {

public:
    Plane(const std::shared_ptr<Shader>& shader, const std::variant<std::shared_ptr<Texture2D>, glm::vec4>& material);
    virtual ~Plane();

    virtual void draw(const std::optional<Camera>& camera = {}) override;

private:   
    virtual void createBuffers() override;

    inline bool hasTexture() const { return m_material.index() == 0; }

    VertexArray m_vertexArray;
    std::variant<std::shared_ptr<Texture2D>, glm::vec4> m_material;

};
