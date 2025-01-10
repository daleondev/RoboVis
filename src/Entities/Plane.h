// #pragma once

// #include "Entity.h"

// #include "Renderer/VertexArray.h"

// class Texture2D;

// class Plane : public EntityOld {

// public:
//     Plane(const std::variant<std::shared_ptr<Texture2D>, glm::vec4>& material);
//     virtual ~Plane();

//     virtual void draw(const Camera& camera) override;
//     virtual void updateTriangulationData() override;

// private:   
//     void createBuffers();

//     inline bool hasTexture() const { return m_material.index() == 0; }

//     std::variant<std::shared_ptr<Texture2D>, glm::vec4> m_material;
//     VertexArray m_vertexArray;

//     inline static constexpr std::array<glm::vec3, 4> s_vertices {
//         glm::vec3{-0.5f, -0.5f, 0.0f},
//         glm::vec3{ 0.5f, -0.5f, 0.0f},
//         glm::vec3{ 0.5f,  0.5f, 0.0f},
//         glm::vec3{-0.5f,  0.5f, 0.0f}
//     };

//     inline static constexpr std::array<std::array<uint16_t, 3>, 2> s_indices = {
//         std::array<uint16_t, 3>{0, 1, 2}, 
//         std::array<uint16_t, 3>{2, 3, 0}
//     };

// };
