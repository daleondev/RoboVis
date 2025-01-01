#pragma once

#include "Entity.h"

#include "Renderer/VertexArray.h"

struct MeshData
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec4 color;
    };

    std::vector<Vertex> vertices;
    std::vector<std::array<uint16_t, 3>> indices;
};

class Mesh : public Entity {

public:
    Mesh(const std::shared_ptr<Shader>& shader, const aiScene* source, const glm::mat4& t_mesh_world = glm::mat4(1.0f));
    virtual ~Mesh();

    virtual void draw(const std::optional<Camera>& camera = {}) override;

    const std::vector<MeshData> getData();

private:
    virtual void createBuffers() override;

    std::vector<MeshData> m_meshData;
    std::vector<VertexArray> m_vertexArrays;
};
