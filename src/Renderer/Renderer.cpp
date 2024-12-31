#include "pch.h"

#include "Renderer.h"

void Renderer::init()
{
    glEnable(GL_DEPTH_TEST);
    glClearDepthf(1.0f);
    glDepthFunc(GL_LESS);
}

void Renderer::clear(const glm::vec4& clearColor)
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(const std::shared_ptr<Shader>& shader, const VertexArray& vertexArray)
{
    shader->bind();
    vertexArray.bind();
    glDrawElements(GL_TRIANGLES, vertexArray.getIndexBuffer()->getCount(), GL_UNSIGNED_SHORT, 0);
}
