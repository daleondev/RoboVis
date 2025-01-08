#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"

#include "Scene/Camera.h"

struct PlaneData
{
    struct Vertex
    {
        glm::vec3 position;
		glm::vec4 color;
		/*glm::vec2 texCoord;*/
    };

    static constexpr const std::array<glm::vec3, 4> vertices = {
        glm::vec3{-0.5f, -0.5f, 0.0f},
        glm::vec3{ 0.5f, -0.5f, 0.0f},
        glm::vec3{ 0.5f,  0.5f, 0.0f},
        glm::vec3{-0.5f,  0.5f, 0.0f}
    };
    static constexpr const std::array<std::array<uint16_t, 3>, 2> indices = {
        std::array<uint16_t, 3>{0, 1, 2}, 
        std::array<uint16_t, 3>{2, 3, 0}
    };
};

struct RenderData
{
    std::shared_ptr<VertexArray> vertexArray;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture2D> texture;
};

class Renderer {

public:
    static void init();
    static void shutdown();

    static void begin();
    static void end();

    static void clear(const glm::vec4& clearColor);
    static void draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

    static void drawPlane(const glm::mat4& transform, const std::variant<glm::vec4, std::shared_ptr<Texture2D>>& material, const Camera& camera);

private:
    static RenderData s_planeData;
    static RenderData s_frameData;
    static RenderData s_meshData;
    static RenderData s_sphereData;

};
