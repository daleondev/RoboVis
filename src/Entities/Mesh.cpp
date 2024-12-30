#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <iostream>
#include <ranges>

Mesh::Mesh(const std::shared_ptr<Shader>& shader, const aiScene* source)
    : Entity(shader)
{
    m_meshData.resize(source->mNumMeshes);
    for (size_t i = 0; i < m_meshData.size(); ++i) {
        auto& meshData = m_meshData[i];
        auto& meshSource = source->mMeshes[i];

        glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);
        if (source->mNumMaterials > 0 && meshSource->mMaterialIndex < source->mNumMaterials) {
            uint32_t size; 
            source->mMaterials[meshSource->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, glm::value_ptr(color), &size);
        }

        meshData.vertices.resize(meshSource->mNumVertices);
        for (size_t j = 0; j < meshData.vertices.size(); ++j) {
            auto& vertex = meshData.vertices[j];
            auto& vertSource = meshSource->mVertices[j];

            vertex.pos.x = vertSource.x;
            vertex.pos.y = vertSource.y;
            vertex.pos.z = vertSource.z;
            vertex.color = color;
        }

        meshData.indices.resize(meshSource->mNumFaces);
        for (size_t j = 0; j < meshData.indices.size(); ++j) {
            auto& indices = meshData.indices[j];
            auto& indexSource = meshSource->mFaces[j];

            if (indexSource.mNumIndices < 3)
                continue;

            for (size_t k = 0; k < 3; ++k)
                indices[k] = indexSource.mIndices[k];
        }
    }

    createBuffers();
}

Mesh::~Mesh() = default;

void Mesh::draw(const std::optional<Camera>& camera)
{
    if (camera)
        updateMvp(camera);

    for (const auto& va : m_vertexArrays)
        Renderer::draw(m_shader, va);
}

const std::vector<MeshData>& Mesh::getData()
{
    for (auto& mesh : m_meshData) {
        for (auto& v : mesh.vertices) {
            v.pos = glm::vec4(v.pos, 1.0f) * m_model;
        }
    }

    return m_meshData;
}

void Mesh::createBuffers()
{
    m_vertexArrays.resize(m_meshData.size());
    for (size_t i = 0; i < m_vertexArrays.size(); ++i) {
        auto& vertices = m_meshData[i].vertices;
        auto& indices = m_meshData[i].indices;
        auto& vertexArray = m_vertexArrays[i];

        std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>();
        vertexBuffer->allocate(reinterpret_cast<float*>(vertices.data()), vertices.size() * sizeof(MeshData::Vertex)/sizeof(float));
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_position" },
            { ShaderDataType::Float4, "a_color" }
        };
        vertexBuffer->setLayout(layout);
        vertexArray.addVertexBuffer(vertexBuffer);

        std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>();
        indexBuffer->allocate(indices.data()->data(), 3*indices.size());
        vertexArray.setIndexBuffer(indexBuffer);
    }
}