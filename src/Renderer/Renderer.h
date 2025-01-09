#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "RenderData.h"

#include "Scene/Camera.h"

class Renderer {

public:
    static void init();
    static void shutdown();

    static void clear(const glm::vec4& clearColor);

    static void draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);
    static void drawPlane(const glm::mat4& transform, const glm::vec4& color);
    static void drawPlane(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture);
    static void drawFrame(const glm::mat4& transform);

private:
    static RenderData s_planeData;
    static RenderData s_frameData;
    static RenderData s_meshData;
    static RenderData s_sphereData;

};
