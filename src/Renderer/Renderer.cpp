#include "pch.h"

#include "Renderer.h"

RenderData Renderer::s_planeData;
RenderData Renderer::s_frameData;
RenderData Renderer::s_meshData;
RenderData Renderer::s_sphereData;

void Renderer::init()
{
    glEnable(GL_DEPTH_TEST);
    glClearDepthf(1.0f);
    glDepthFunc(GL_LESS);

    if (ShaderLibrary::exists("FlatColor"))
        s_planeData.shader = ShaderLibrary::get("FlatColor");
    else
        s_planeData.shader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/FlatColor", "FlatColor");

    s_planeData.vertexArray = std::make_shared<VertexArray>();
    s_planeData.vertexBuffer = std::make_shared<VertexBuffer>();
    s_planeData.vertexBuffer->allocate(reinterpret_cast<const float*>(PlaneData::vertices.data()), PlaneData::vertices.size() * 3);
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_position" },
        // { ShaderDataType::Float4, "a_color" },
        /*{ ShaderDataType::Float2, "a_texCoord" }*/
    };
    s_planeData.vertexBuffer->setLayout(layout);
    s_planeData.vertexArray->addVertexBuffer(s_planeData.vertexBuffer);

    s_planeData.indexBuffer = std::make_shared<IndexBuffer>();
    s_planeData.indexBuffer->allocate(reinterpret_cast<const uint16_t*>(PlaneData::indices.data()), PlaneData::indices.size() * 3);
    s_planeData.vertexArray->setIndexBuffer(s_planeData.indexBuffer);
}

void Renderer::begin()
{

}

void Renderer::end()
{

}

void Renderer::clear(const glm::vec4& clearColor)
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
{
    shader->bind();
    vertexArray->bind();
    glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_SHORT, 0);
}

void Renderer::drawPlane(const glm::mat4& transform, const std::variant<glm::vec4, std::shared_ptr<Texture2D>>& material, const Camera& camera)
{
    glm::mat4 mvp = camera.getProjection() * camera.getView() * glm::transpose(transform);

    s_planeData.shader->bind();
    s_planeData.shader->uploadMat4("u_mvp", mvp);
    s_planeData.vertexArray->bind();
    glDrawElements(GL_TRIANGLES, s_planeData.vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_SHORT, 0);
}