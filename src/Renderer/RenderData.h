#pragma once

#include "pch.h"

struct PlaneData
{
    struct Vertex
    {
        glm::vec3 position;
		glm::vec2 texCoord;
    };

    inline static const BufferLayout layout = {
        { ShaderDataType::Float3, "a_position" },
        { ShaderDataType::Float2, "a_texCoord" }
    };

    static constexpr const std::array<Vertex, 4> vertices = {
        Vertex{glm::vec3{-0.5f, -0.5f, 0.0f},    glm::vec2{0.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f, -0.5f, 0.0f},    glm::vec2{1.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f,  0.5f, 0.0f},    glm::vec2{1.0f, 1.0f}},
        Vertex{glm::vec3{-0.5f,  0.5f, 0.0f},    glm::vec2{0.0f, 1.0f}}
    };
    static constexpr const std::array<std::array<uint16_t, 3>, 2> indices = {
        std::array<uint16_t, 3>{0, 1, 2}, 
        std::array<uint16_t, 3>{2, 3, 0}
    };
};

struct FrameData
{
    struct Vertex
    {
        glm::vec3 position;
		glm::vec4 color;
    };

    inline static const BufferLayout layout = {
        { ShaderDataType::Float3, "a_position" },
        { ShaderDataType::Float4, "a_color" }
    };

    static constexpr std::array<Vertex, 8 * 3> vertices = {
        Vertex{glm::vec3{-0.025f, -0.025f,  0.025f},      glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{ 1.0f,   -0.025f,  0.025f},      glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{ 1.0f,    0.025f,  0.025f},      glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{-0.025f,  0.025f,  0.025f},      glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{-0.025f, -0.025f, -0.025f},      glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{-0.025f,  0.025f, -0.025f},      glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{ 1.0f,   -0.025f, -0.025f},      glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{ 1.0f,    0.025f, -0.025f},      glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},

        Vertex{glm::vec3{-0.025f, -0.025f,  0.025f},      glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{ 0.025f, -0.025f,  0.025f},      glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{ 0.025f,  1.0f,    0.025f},      glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{-0.025f,  1.0f,    0.025f},      glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{-0.025f, -0.025f, -0.025f},      glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{-0.025f,  1.0f,   -0.025f},      glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{ 0.025f, -0.025f, -0.025f},      glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{glm::vec3{ 0.025f,  1.0f,   -0.025f},      glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},

        Vertex{glm::vec3{-0.025f, -0.025f,    1.0f},      glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec3{ 0.025f, -0.025f,    1.0f},      glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec3{ 0.025f,  0.025f,    1.0f},      glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec3{-0.025f,  0.025f,    1.0f},      glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec3{-0.025f, -0.025f, -0.025f},      glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec3{-0.025f,  0.025f, -0.025f},      glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec3{ 0.025f, -0.025f, -0.025f},      glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec3{ 0.025f,  0.025f, -0.025f},      glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}}    
    };

    static constexpr std::array<std::array<uint16_t, 3>, 12 * 3> indices = {
            std::array<uint16_t, 3>{8 * 0 + 0, 8 * 0 + 1, 8 * 0 + 2},
            std::array<uint16_t, 3>{8 * 0 + 2, 8 * 0 + 3, 8 * 0 + 0},
            std::array<uint16_t, 3>{8 * 0 + 4, 8 * 0 + 0, 8 * 0 + 3},
            std::array<uint16_t, 3>{8 * 0 + 3, 8 * 0 + 5, 8 * 0 + 4},
            std::array<uint16_t, 3>{8 * 0 + 6, 8 * 0 + 4, 8 * 0 + 5},
            std::array<uint16_t, 3>{8 * 0 + 5, 8 * 0 + 7, 8 * 0 + 6},
            std::array<uint16_t, 3>{8 * 0 + 1, 8 * 0 + 6, 8 * 0 + 7},
            std::array<uint16_t, 3>{8 * 0 + 7, 8 * 0 + 2, 8 * 0 + 1},
            std::array<uint16_t, 3>{8 * 0 + 4, 8 * 0 + 6, 8 * 0 + 1},
            std::array<uint16_t, 3>{8 * 0 + 1, 8 * 0 + 0, 8 * 0 + 4},
            std::array<uint16_t, 3>{8 * 0 + 5, 8 * 0 + 7, 8 * 0 + 2},
            std::array<uint16_t, 3>{8 * 0 + 2, 8 * 0 + 3, 8 * 0 + 5},

            std::array<uint16_t, 3>{8 * 1 + 0, 8 * 1 + 1, 8 * 1 + 2},
            std::array<uint16_t, 3>{8 * 1 + 2, 8 * 1 + 3, 8 * 1 + 0},
            std::array<uint16_t, 3>{8 * 1 + 4, 8 * 1 + 0, 8 * 1 + 3},
            std::array<uint16_t, 3>{8 * 1 + 3, 8 * 1 + 5, 8 * 1 + 4},
            std::array<uint16_t, 3>{8 * 1 + 6, 8 * 1 + 4, 8 * 1 + 5},
            std::array<uint16_t, 3>{8 * 1 + 5, 8 * 1 + 7, 8 * 1 + 6},
            std::array<uint16_t, 3>{8 * 1 + 1, 8 * 1 + 6, 8 * 1 + 7},
            std::array<uint16_t, 3>{8 * 1 + 7, 8 * 1 + 2, 8 * 1 + 1},
            std::array<uint16_t, 3>{8 * 1 + 4, 8 * 1 + 6, 8 * 1 + 1},
            std::array<uint16_t, 3>{8 * 1 + 1, 8 * 1 + 0, 8 * 1 + 4},
            std::array<uint16_t, 3>{8 * 1 + 5, 8 * 1 + 7, 8 * 1 + 2},
            std::array<uint16_t, 3>{8 * 1 + 2, 8 * 1 + 3, 8 * 1 + 5},

            std::array<uint16_t, 3>{8 * 2 + 0, 8 * 2 + 1, 8 * 2 + 2},
            std::array<uint16_t, 3>{8 * 2 + 2, 8 * 2 + 3, 8 * 2 + 0},
            std::array<uint16_t, 3>{8 * 2 + 4, 8 * 2 + 0, 8 * 2 + 3},
            std::array<uint16_t, 3>{8 * 2 + 3, 8 * 2 + 5, 8 * 2 + 4},
            std::array<uint16_t, 3>{8 * 2 + 6, 8 * 2 + 4, 8 * 2 + 5},
            std::array<uint16_t, 3>{8 * 2 + 5, 8 * 2 + 7, 8 * 2 + 6},
            std::array<uint16_t, 3>{8 * 2 + 1, 8 * 2 + 6, 8 * 2 + 7},
            std::array<uint16_t, 3>{8 * 2 + 7, 8 * 2 + 2, 8 * 2 + 1},
            std::array<uint16_t, 3>{8 * 2 + 4, 8 * 2 + 6, 8 * 2 + 1},
            std::array<uint16_t, 3>{8 * 2 + 1, 8 * 2 + 0, 8 * 2 + 4},
            std::array<uint16_t, 3>{8 * 2 + 5, 8 * 2 + 7, 8 * 2 + 2},
            std::array<uint16_t, 3>{8 * 2 + 2, 8 * 2 + 3, 8 * 2 + 5}
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