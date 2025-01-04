#pragma once

#include "Entity.h"

#include "Renderer/VertexArray.h"

class Sphere : public Entity {

public:
    Sphere(const std::shared_ptr<Shader>& shader, const glm::vec4& color);
    virtual ~Sphere();

    virtual void draw(const std::optional<Camera>& camera = {}) override;

private:   
    virtual void createBuffers() override;

    glm::vec4 m_color;
    VertexArray m_vertexArray;

};
