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

    //------------------------------------------------------
    //                      Plane
    //------------------------------------------------------

    s_planeData.shader = ShaderLibrary::exists("Plane") ?   ShaderLibrary::get("Plane") :
                                                            ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/Plane", "Plane");

    s_planeData.vertexArray = std::make_shared<VertexArray>();
    s_planeData.vertexBuffer = std::make_shared<VertexBuffer>();
    s_planeData.vertexBuffer->allocate(reinterpret_cast<const float*>(PlaneData::vertices.data()), PlaneData::vertices.size() * sizeof(PlaneData::Vertex)/sizeof(float));
    s_planeData.vertexBuffer->setLayout(PlaneData::layout);
    s_planeData.vertexArray->addVertexBuffer(s_planeData.vertexBuffer);

    s_planeData.indexBuffer = std::make_shared<IndexBuffer>();
    s_planeData.indexBuffer->allocate(reinterpret_cast<const uint16_t*>(PlaneData::indices.data()), PlaneData::indices.size() * 3);
    s_planeData.vertexArray->setIndexBuffer(s_planeData.indexBuffer);

    //------------------------------------------------------
    //                      Frame
    //------------------------------------------------------

    s_frameData.shader = ShaderLibrary::exists("Frame") ?   ShaderLibrary::get("Frame") :
                                                            ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/Frame", "Frame");

    s_frameData.vertexArray = std::make_shared<VertexArray>();
    s_frameData.vertexBuffer = std::make_shared<VertexBuffer>();
    s_frameData.vertexBuffer->allocate(reinterpret_cast<const float*>(FrameData::vertices.data()), FrameData::vertices.size() * sizeof(FrameData::Vertex)/sizeof(float));
    s_frameData.vertexBuffer->setLayout(FrameData::layout);
    s_frameData.vertexArray->addVertexBuffer(s_frameData.vertexBuffer);

    s_frameData.indexBuffer = std::make_shared<IndexBuffer>();
    s_frameData.indexBuffer->allocate(reinterpret_cast<const uint16_t*>(FrameData::indices.data()), FrameData::indices.size() * 3);
    s_frameData.vertexArray->setIndexBuffer(s_frameData.indexBuffer);
}

void Renderer::shutdown()
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

void Renderer::drawPlane(const glm::mat4& transform, const glm::vec4& color)
{
    const auto& camera = CameraController::getCamera();
    glm::mat4 mvp = camera.getProjection() * camera.getView() * glm::transpose(transform);

    s_planeData.shader->bind();
    s_planeData.shader->uploadMat4("u_mvp", mvp);
    s_planeData.shader->uploadVec4("u_color", color);
    s_planeData.shader->uploadInt("u_useTex", 0);
    draw(s_planeData.shader, s_planeData.vertexArray);
}

void Renderer::drawPlane(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture)
{
    const auto& camera = CameraController::getCamera();
    glm::mat4 mvp = camera.getProjection() * camera.getView() * glm::transpose(transform);

    s_planeData.shader->bind();
    s_planeData.shader->uploadMat4("u_mvp", mvp);
    s_planeData.shader->uploadInt("u_texture", texture->getSlot());
    s_planeData.shader->uploadInt("u_useTex", 1);
    texture->bind();
    draw(s_planeData.shader, s_planeData.vertexArray);
}

void Renderer::drawFrame(const glm::mat4& transform)
{
    const auto& camera = CameraController::getCamera();
    glm::mat4 mvp = camera.getProjection() * camera.getView() * glm::transpose(transform);

    s_frameData.shader->bind();
    s_frameData.shader->uploadMat4("u_mvp", mvp);
    draw(s_frameData.shader, s_frameData.vertexArray);
}