#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <iostream>
#include <ranges>

Mesh::Mesh(const std::shared_ptr<Shader>& shader, aiMesh* data, aiMaterial* material)
    : Entity(shader), m_data(data), m_material(material)
{
    createBuffers();
}

Mesh::~Mesh() = default;

void Mesh::draw(const std::optional<Camera>& camera)
{
    if (camera)
        updateMvp(camera);

    Renderer::draw(m_shader, m_vertexArray);
}

void Mesh::createBuffers()
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec4 color;
    };

    std::vector<Vertex> vertices(m_data->mNumVertices);
    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto& pos = m_data->mVertices[i];
        
        vertices[i].position = { pos.x, pos.y, pos.z };

        if (m_material) {
            uint size;            
            m_material->Get(AI_MATKEY_COLOR_DIFFUSE, glm::value_ptr(vertices[i].color), &size);
        } else {
            vertices[i].color = {0.0f, 0.0f, 1.0f, 1.0f};
        }      
    }

    std::vector<GLushort> indices(m_data->mNumFaces * 3);
    size_t i = 0;
    for (size_t j = 0; j < m_data->mNumFaces; ++j) { 
        if (m_data->mFaces[j].mNumIndices < 3)
            continue;

        indices[i++] = m_data->mFaces[j].mIndices[0];
        indices[i++] = m_data->mFaces[j].mIndices[1];
        indices[i++] = m_data->mFaces[j].mIndices[2];
    }

    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->allocate(reinterpret_cast<float*>(vertices.data()), vertices.size() * sizeof(Vertex)/sizeof(float));
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_position" },
        { ShaderDataType::Float4, "a_color" }
    };
    vertexBuffer->setLayout(layout);
    m_vertexArray.addVertexBuffer(vertexBuffer);

    std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->allocate(indices.data(), indices.size());
    m_vertexArray.setIndexBuffer(indexBuffer);
}