#pragma once

#include "Entity.h"
#include "Camera.h"

class Marker : public Entity {

public:
    Marker(const std::shared_ptr<Shader>& shader);
    virtual ~Marker();

    virtual void create() override;
    virtual void draw(const std::optional<Camera>& camera = {}) override;

private:
    virtual void createBuffers() override;
};
