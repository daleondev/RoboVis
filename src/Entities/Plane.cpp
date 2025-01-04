#include "pch.h"

#include "Plane.h"

#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"

Plane::Plane(const std::shared_ptr<Shader>& shader, const std::variant<std::shared_ptr<Texture2D>, glm::vec4>& material)
    : Entity(shader), m_material(material)
{
    createBuffers();
}

Plane::~Plane()
{
    m_vertexArray.release();
}

void Plane::draw(const std::optional<Camera>& camera)
{
    if (!m_visible)
        return;

    if (camera)
        updateMvp(camera);

    if (hasTexture())
        std::get<std::shared_ptr<Texture2D>>(m_material)->bind();
        
    Renderer::draw(m_shader, m_vertexArray);
}

PlaneData Plane::getData() const
{
    std::array<glm::vec3, 4> vertices {
        m_model * glm::vec4{-0.5f, -0.5f, 0.0f, 1.0f},
        m_model * glm::vec4{ 0.5f, -0.5f, 0.0f, 1.0f},
        m_model * glm::vec4{ 0.5f,  0.5f, 0.0f, 1.0f},
        m_model * glm::vec4{-0.5f,  0.5f, 0.0f, 1.0f}
    };

    constexpr std::array<std::array<uint16_t, 3>, 2> indices = {
        std::array<uint16_t, 3>{0, 1, 2}, 
        std::array<uint16_t, 3>{2, 3, 0}
    };

    return PlaneData{vertices, indices};
}

void Plane::createBuffers()
{
    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>();
    if (hasTexture()) {
        constexpr std::array<GLfloat, 5 * 4> vertices {
            -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
             0.5f, -0.5f, 0.0f,     1.0f, 0.0f,
             0.5f,  0.5f, 0.0f,     1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,     0.0f, 1.0f
        };

        vertexBuffer->allocate(vertices.data(), vertices.size());
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_position" },
            { ShaderDataType::Float2, "a_texCoord" }
        };
        vertexBuffer->setLayout(layout);
    }
    else {
        const auto color = std::get<glm::vec4>(m_material);
        const std::array<GLfloat, 7 * 4> vertices {
            -0.5f, -0.5f, 0.0f,     color.r, color.g, color.b, color.a,
             0.5f, -0.5f, 0.0f,     color.r, color.g, color.b, color.a,
             0.5f,  0.5f, 0.0f,     color.r, color.g, color.b, color.a,
            -0.5f,  0.5f, 0.0f,     color.r, color.g, color.b, color.a
        };

        vertexBuffer->allocate(vertices.data(), vertices.size());
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_position" },
            { ShaderDataType::Float4, "a_color" }
        };
        vertexBuffer->setLayout(layout);
    }
    m_vertexArray.addVertexBuffer(vertexBuffer);

    constexpr std::array<GLushort, 6> indices = {
        0, 1, 2, 
        2, 3, 0
    };

    std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->allocate(indices.data(), indices.size());
    m_vertexArray.setIndexBuffer(indexBuffer);
}