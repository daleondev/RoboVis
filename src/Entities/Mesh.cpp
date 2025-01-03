#include "pch.h"

#include "Mesh.h"

#include "Renderer/Renderer.h"

#include "Util/geometry.h"

Mesh::Mesh(const std::shared_ptr<Shader>& shader, const aiScene* source, const glm::mat4& t_mesh_world)
    : Entity(shader)
{
    const aiMatrix4x4 rootTransform = source->mRootNode->mTransformation;
    glm::mat4 t_root_world;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            t_root_world[i][j] = rootTransform[i][j];

    m_meshData.resize(source->mNumMeshes);
    for (size_t i = 0; i < m_meshData.size(); ++i) {
        auto& meshData = m_meshData[i];
        auto& meshSource = source->mMeshes[i];

        glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);
        if (source->mNumMaterials > 0 && meshSource->mMaterialIndex < source->mNumMaterials) {
            uint32_t size; 
            source->mMaterials[meshSource->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, glm::value_ptr(color), &size);
        }
        else 
            LOG_WARN << "Material not valid: " << meshSource->mMaterialIndex;

        meshData.vertices.resize(meshSource->mNumVertices);
        for (size_t j = 0; j < meshData.vertices.size(); ++j) {
            auto& vertex = meshData.vertices[j];
            auto& vertSource = meshSource->mVertices[j];

            glm::vec3 p_vertex_world;
            p_vertex_world.x = vertSource.x;
            p_vertex_world.y = vertSource.y;
            p_vertex_world.z = vertSource.z;  

            p_vertex_world = glm::vec4(p_vertex_world, 1.0f) * t_root_world * t_mesh_world;

            vertex.pos = p_vertex_world;
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

const std::vector<MeshData> Mesh::getData()
{
    auto meshData = m_meshData;
    for (auto& mesh : meshData) {
        for (auto& v : mesh.vertices) {
            v.pos = m_model * glm::vec4(v.pos, 1.0f);
        }
    }
    return meshData;
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