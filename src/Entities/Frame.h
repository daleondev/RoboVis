#pragma once

#include "Entity.h"

#include "Renderer/VertexArray.h"

class Frame : public Entity {

public:
    Frame(const std::shared_ptr<Shader>& shader);
    virtual ~Frame();

    virtual void draw(const std::optional<Camera>& camera = {}) override;

private:
    virtual void createBuffers() override;

    VertexArray m_vertexArray;
};
