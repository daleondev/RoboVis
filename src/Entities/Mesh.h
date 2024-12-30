#pragma once

#include "Entity.h"
#include "Camera.h"

#include <assimp/mesh.h>
#include <assimp/material.h>

class Mesh : public Entity {

public:
    Mesh(const std::shared_ptr<Shader>& shader, aiMesh* data, aiMaterial* material);
    virtual ~Mesh();

    virtual void draw(const std::optional<Camera>& camera = {}) override;

private:
    virtual void createBuffers() override;

    glm::mat4 m_model;
    aiMesh* m_data;
    aiMaterial* m_material;
};
