// #include "pch.h"

// #include "Sphere.h"

// #include "Renderer/Renderer.h"

// #include "Util/Log.h"
// #include "Util/geometry.h"

// Sphere::Sphere(const glm::vec4& color)
//     : m_color(color)
// {
//     if (ShaderLibrary::exists("FlatColor"))
//         m_shader = ShaderLibrary::get("FlatColor");
//     else
//         m_shader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/FlatColor", "FlatColor");

//     createBuffers();
// }

// Sphere::~Sphere()
// {
//     m_vertexArray.release();
// }

// void Sphere::draw(const Camera& camera)
// {
//     if (!m_visible)
//         return;

//     updateMvp(camera);

//     m_shader->uploadVec4("u_color", m_color);
//     // Renderer::draw(m_shader, m_vertexArray);
// }

// // Constants for the sphere
// static constexpr float SPHERE_RADIUS = 0.5f;
// static constexpr uint16_t SPHERE_RINGS = 16;
// static constexpr uint16_t SPHERE_SECTORS = 32;

// // Function to generate sphere data with triangles
// static constexpr auto generateSphere() 
// {
//     std::pair<std::array<float, (SPHERE_RINGS * SPHERE_SECTORS * 8)>, std::array<uint16_t, ((SPHERE_RINGS - 1) * SPHERE_SECTORS * 6)>> sphereData;

//     constexpr float R = 1.0f / (float)(SPHERE_RINGS - 1);
//     constexpr float S = 1.0f / (float)(SPHERE_SECTORS - 1);
//     constexpr float PI = 3.14159265359f;

//     uint16_t vertexIndex = 0;
//     uint16_t indexIndex = 0;

//     // Generate vertices
//     for (uint16_t r = 0; r < SPHERE_RINGS; ++r) {
//         for (uint16_t s = 0; s < SPHERE_SECTORS; ++s) {
//             const float y = std::sin(-PI / 2.0f + PI * r * R);              // Y coordinate
//             const float x = std::cos(2.0f * PI * s * S) * std::sin(PI * r * R); // X coordinate
//             const float z = std::sin(2.0f * PI * s * S) * std::sin(PI * r * R); // Z coordinate

//             // Vertex position
//             sphereData.first[vertexIndex++] = x * SPHERE_RADIUS;
//             sphereData.first[vertexIndex++] = y * SPHERE_RADIUS;
//             sphereData.first[vertexIndex++] = z * SPHERE_RADIUS;
//         }
//     }

//     // Generate indices for triangles
//     for (uint16_t r = 0; r < SPHERE_RINGS - 1; ++r) {
//         for (uint16_t s = 0; s < SPHERE_SECTORS; ++s) {
//             uint16_t i1 = r * SPHERE_SECTORS + s;
//             uint16_t i2 = r * SPHERE_SECTORS + (s + 1) % SPHERE_SECTORS;
//             uint16_t i3 = (r + 1) * SPHERE_SECTORS + s;
//             uint16_t i4 = (r + 1) * SPHERE_SECTORS + (s + 1) % SPHERE_SECTORS;

//             // First triangle
//             sphereData.second[indexIndex++] = i1;
//             sphereData.second[indexIndex++] = i2;
//             sphereData.second[indexIndex++] = i4;

//             // Second triangle
//             sphereData.second[indexIndex++] = i1;
//             sphereData.second[indexIndex++] = i4;
//             sphereData.second[indexIndex++] = i3;
//         }
//     }

//     return sphereData;
// }

// void Sphere::createBuffers()
// {
//     auto sphereData = generateSphere();

//     std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>();
//     vertexBuffer->allocate(sphereData.first.data(), sphereData.first.size());
//     BufferLayout layout = {
//         { ShaderDataType::Float3, "a_position" }
//     };
//     vertexBuffer->setLayout(layout);
//     m_vertexArray.addVertexBuffer(vertexBuffer);

//     std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>();
//     indexBuffer->allocate(sphereData.second.data(), sphereData.second.size());
//     m_vertexArray.setIndexBuffer(indexBuffer);
// }