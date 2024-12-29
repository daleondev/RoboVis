#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"

class Renderer {

public:
    static void init();

    static void clear(const glm::vec4& clearColor);
    static void draw(const std::shared_ptr<Shader>& shader, const VertexArray& vertexArray);
};
