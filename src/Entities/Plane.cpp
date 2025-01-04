#include "pch.h"

#include "Plane.h"

#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"

Plane::Plane(const std::variant<std::shared_ptr<Texture2D>, glm::vec4>& material)
    : m_material(material)
{
    if (hasTexture())
        if (ShaderLibrary::exists("Texture"))
            m_shader = ShaderLibrary::get("Texture");
        else
            m_shader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/Texture", "Texture");
    else
        if (ShaderLibrary::exists("FlatColor"))
            m_shader = ShaderLibrary::get("FlatColor");
        else
            m_shader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/FlatColor", "FlatColor");

    m_triData = std::make_shared<TriangulationData>();
    m_triData->indices = std::vector {
        s_indices[0],
        s_indices[1]
    };
    m_triData->vertices.resize(s_vertices.size());
    updateTriangulationData();

    createBuffers();
}

Plane::~Plane()
{
    m_vertexArray.release();
}

void Plane::draw(const Camera& camera)
{
    if (!m_visible)
        return;

    updateMvp(camera);

    if (hasTexture())
        std::get<std::shared_ptr<Texture2D>>(m_material)->bind();
    else
        m_shader->uploadVec4("u_color", std::get<glm::vec4>(m_material));
        
    Renderer::draw(m_shader, m_vertexArray);
}

void Plane::updateTriangulationData()
{
    m_triData->vertices[0] = glm::vec4{s_vertices[0], 1.0f} * m_model;
    m_triData->vertices[1] = glm::vec4{s_vertices[1], 1.0f} * m_model;
    m_triData->vertices[2] = glm::vec4{s_vertices[2], 1.0f} * m_model;
    m_triData->vertices[3] = glm::vec4{s_vertices[3], 1.0f} * m_model;
}

void Plane::createBuffers()
{
    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>();
    if (hasTexture()) {
        constexpr std::array<GLfloat, 5 * 4> vertices {
            s_vertices[0][0], s_vertices[0][1], s_vertices[0][2],   0.0f, 0.0f,
            s_vertices[1][0], s_vertices[1][1], s_vertices[1][2],   1.0f, 0.0f,
            s_vertices[2][0], s_vertices[2][1], s_vertices[2][2],   1.0f, 1.0f,
            s_vertices[3][0], s_vertices[3][1], s_vertices[3][2],   0.0f, 1.0f
        };

        vertexBuffer->allocate(vertices.data(), vertices.size());
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_position" },
            { ShaderDataType::Float2, "a_texCoord" }
        };
        vertexBuffer->setLayout(layout);
    }
    else {
        vertexBuffer->allocate(reinterpret_cast<const GLfloat*>(s_vertices.data()), 3*s_vertices.size());
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_position" }
        };
        vertexBuffer->setLayout(layout);
    }
    m_vertexArray.addVertexBuffer(vertexBuffer);

    std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->allocate(reinterpret_cast<const GLushort*>(s_indices.data()), 3*s_indices.size());
    m_vertexArray.setIndexBuffer(indexBuffer);
}