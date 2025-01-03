#include "pch.h"

#include "Frame.h"

#include "Renderer/Renderer.h"

Frame::Frame(const std::shared_ptr<Shader>& shader)
    : Entity(shader)
{
    createBuffers();
}

Frame::~Frame()
{
    m_vertexArray.release();
}

void Frame::draw(const std::optional<Camera>& camera)
{
    if (!m_visible)
        return;

    if (camera)
        updateMvp(camera);
        
    Renderer::draw(m_shader, m_vertexArray);
}

void Frame::createBuffers()
{
    constexpr std::array<GLfloat, 8 * 3 * 7> vertices = {
        -0.025f, -0.025f,  0.025f,      1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,   -0.025f,  0.025f,      1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,    0.025f,  0.025f,      1.0f, 0.0f, 0.0f, 1.0f,
        -0.025f,  0.025f,  0.025f,      1.0f, 0.0f, 0.0f, 1.0f,
        -0.025f, -0.025f, -0.025f,      1.0f, 0.0f, 0.0f, 1.0f,
        -0.025f,  0.025f, -0.025f,      1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,   -0.025f, -0.025f,      1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,    0.025f, -0.025f,      1.0f, 0.0f, 0.0f, 1.0f,

        -0.025f, -0.025f,  0.025f,      0.0f, 1.0f, 0.0f, 1.0f,
         0.025f, -0.025f,  0.025f,      0.0f, 1.0f, 0.0f, 1.0f,
         0.025f,  1.0f,    0.025f,      0.0f, 1.0f, 0.0f, 1.0f,
        -0.025f,  1.0f,    0.025f,      0.0f, 1.0f, 0.0f, 1.0f,
        -0.025f, -0.025f, -0.025f,      0.0f, 1.0f, 0.0f, 1.0f,
        -0.025f,  1.0f,   -0.025f,      0.0f, 1.0f, 0.0f, 1.0f,
         0.025f, -0.025f, -0.025f,      0.0f, 1.0f, 0.0f, 1.0f,
         0.025f,  1.0f,   -0.025f,      0.0f, 1.0f, 0.0f, 1.0f,

        -0.025f, -0.025f,  1.0f,        0.0f, 0.0f, 1.0f, 1.0f,
         0.025f, -0.025f,  1.0f,        0.0f, 0.0f, 1.0f, 1.0f,
         0.025f,  0.025f,  1.0f,        0.0f, 0.0f, 1.0f, 1.0f,
        -0.025f,  0.025f,  1.0f,        0.0f, 0.0f, 1.0f, 1.0f,
        -0.025f, -0.025f, -0.025f,      0.0f, 0.0f, 1.0f, 1.0f,
        -0.025f,  0.025f, -0.025f,      0.0f, 0.0f, 1.0f, 1.0f,
         0.025f, -0.025f, -0.025f,      0.0f, 0.0f, 1.0f, 1.0f,
         0.025f,  0.025f, -0.025f,      0.0f, 0.0f, 1.0f, 1.0f    
    };

    constexpr std::array<GLushort, 12 * 3 * 3> indices = {
            8 * 0 + 0, 8 * 0 + 1, 8 * 0 + 2,
            8 * 0 + 2, 8 * 0 + 3, 8 * 0 + 0,
            8 * 0 + 4, 8 * 0 + 0, 8 * 0 + 3,
            8 * 0 + 3, 8 * 0 + 5, 8 * 0 + 4,
            8 * 0 + 6, 8 * 0 + 4, 8 * 0 + 5,
            8 * 0 + 5, 8 * 0 + 7, 8 * 0 + 6,
            8 * 0 + 1, 8 * 0 + 6, 8 * 0 + 7,
            8 * 0 + 7, 8 * 0 + 2, 8 * 0 + 1,
            8 * 0 + 4, 8 * 0 + 6, 8 * 0 + 1,
            8 * 0 + 1, 8 * 0 + 0, 8 * 0 + 4,
            8 * 0 + 5, 8 * 0 + 7, 8 * 0 + 2,
            8 * 0 + 2, 8 * 0 + 3, 8 * 0 + 5,

            8 * 1 + 0, 8 * 1 + 1, 8 * 1 + 2,
            8 * 1 + 2, 8 * 1 + 3, 8 * 1 + 0,
            8 * 1 + 4, 8 * 1 + 0, 8 * 1 + 3,
            8 * 1 + 3, 8 * 1 + 5, 8 * 1 + 4,
            8 * 1 + 6, 8 * 1 + 4, 8 * 1 + 5,
            8 * 1 + 5, 8 * 1 + 7, 8 * 1 + 6,
            8 * 1 + 1, 8 * 1 + 6, 8 * 1 + 7,
            8 * 1 + 7, 8 * 1 + 2, 8 * 1 + 1,
            8 * 1 + 4, 8 * 1 + 6, 8 * 1 + 1,
            8 * 1 + 1, 8 * 1 + 0, 8 * 1 + 4,
            8 * 1 + 5, 8 * 1 + 7, 8 * 1 + 2,
            8 * 1 + 2, 8 * 1 + 3, 8 * 1 + 5,

            8 * 2 + 0, 8 * 2 + 1, 8 * 2 + 2,
            8 * 2 + 2, 8 * 2 + 3, 8 * 2 + 0,
            8 * 2 + 4, 8 * 2 + 0, 8 * 2 + 3,
            8 * 2 + 3, 8 * 2 + 5, 8 * 2 + 4,
            8 * 2 + 6, 8 * 2 + 4, 8 * 2 + 5,
            8 * 2 + 5, 8 * 2 + 7, 8 * 2 + 6,
            8 * 2 + 1, 8 * 2 + 6, 8 * 2 + 7,
            8 * 2 + 7, 8 * 2 + 2, 8 * 2 + 1,
            8 * 2 + 4, 8 * 2 + 6, 8 * 2 + 1,
            8 * 2 + 1, 8 * 2 + 0, 8 * 2 + 4,
            8 * 2 + 5, 8 * 2 + 7, 8 * 2 + 2,
            8 * 2 + 2, 8 * 2 + 3, 8 * 2 + 5
    };

    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->allocate(vertices.data(), vertices.size());
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