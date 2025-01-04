#pragma once

#include "Entity.h"

#include "Renderer/VertexArray.h"

struct BoundingBoxData
{
    std::array<glm::vec3, 8> vertices;
    static constexpr std::array<std::array<uint16_t, 3>, 12> indices = {
        std::array<uint16_t, 3>{0, 1, 2}, 
        std::array<uint16_t, 3>{0, 2, 3},
        std::array<uint16_t, 3>{4, 6, 5}, 
        std::array<uint16_t, 3>{4, 7, 6},
        std::array<uint16_t, 3>{0, 3, 7}, 
        std::array<uint16_t, 3>{0, 7, 4},
        std::array<uint16_t, 3>{1, 5, 6}, 
        std::array<uint16_t, 3>{1, 6, 2},
        std::array<uint16_t, 3>{3, 2, 6}, 
        std::array<uint16_t, 3>{3, 6, 7},
        std::array<uint16_t, 3>{0, 4, 5}, 
        std::array<uint16_t, 3>{0, 5, 1}
    };
};

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
    Mesh(const aiScene* source, const glm::mat4& t_mesh_world = glm::mat4(1.0f));
    virtual ~Mesh();

    virtual void draw(const Camera& camera) override;
    virtual void updateTriangulationData() override;

    inline virtual bool rayIntersection(const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist) const override;

private:
    void updateBoundingBox();
    void drawBoundingBox(const Camera& camera);

    void addNode(const aiScene* source, const aiNode* node, glm::mat4 t_node_world, const glm::mat4& t_mesh_world);

    virtual void createBuffers() override;

    std::vector<MeshData> m_meshData;
    std::vector<VertexArray> m_vertexArrays;
    
    std::shared_ptr<Shader> m_shaderBB;
    glm::vec2 m_limitsX;
    glm::vec2 m_limitsY;
    glm::vec2 m_limitsZ;
    BoundingBoxData m_bbData;
    VertexArray m_vertexArrayBB;   
};
