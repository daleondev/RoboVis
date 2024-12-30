#pragma once

#include "Entity.h"
#include "Camera.h"

class Marker : public Entity {

public:
    Marker(const std::shared_ptr<Shader>& shader);
    virtual ~Marker();

    virtual void draw(const std::optional<Camera>& camera = {}) override;

private:
    virtual void createBuffers() override;

    VertexArray m_vertexArray;
};
