#pragma once

#include "Util/util.h"
#include "Util/geometry.h"

#include "Renderer/RenderData.h"

//------------------------------------------------------
//                  Identification
//------------------------------------------------------

struct IdComponent
{
    UUID id;

    IdComponent() : id{uuid()} {}
    IdComponent(const IdComponent&) = default;
};

struct TagComponent
{
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : tag{tag} {}
};

//------------------------------------------------------
//                    Transform
//------------------------------------------------------

struct TransformComponent
{
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;

    TransformComponent() : translation(0.0f), rotation(0.0f), scale(1.0f) {};
    TransformComponent(const TransformComponent&) = default;

    void set(const glm::mat4& transformation)
    {
        translation = getMat4Translation(transformation);

        const auto axisX = getMat4AxisX(transformation);
        const auto axisY = getMat4AxisY(transformation);
        const auto axisZ = getMat4AxisZ(transformation);
        scale.x = glm::length(axisX);
        scale.y = glm::length(axisY);
        scale.z = glm::length(axisZ);

        const auto r = getMat4Rotation(transformation);
        rotation = toEulerZYX(r);
    }

    glm::mat4 get() const
    {
        return glm::translate(glm::mat4(1.0f), translation)
            * glm::mat4(eulerZYX(rotation))
            * glm::scale(glm::mat4(1.0f), scale);
    }
};

//------------------------------------------------------
//                      Visibility
//------------------------------------------------------

struct VisibilityComponent
{
    bool visible;

    VisibilityComponent() : visible{true} {}
    VisibilityComponent(const VisibilityComponent&) = default;
};

//------------------------------------------------------
//                      Bounding Box
//------------------------------------------------------

struct BoundingBoxData
{
    std::array<glm::vec3, 8> vertices;
    static constexpr std::array<std::array<uint16_t, 3>, 12> indices = BoxData::indices;
};

struct BoundingBoxComponent
{
    std::unique_ptr<BoundingBoxData> data;
    glm::vec4 color;

    BoundingBoxComponent() : data{std::make_unique<BoundingBoxData>()}, color(1.0f) {}
    BoundingBoxComponent(const BoundingBoxComponent&) = default;
    BoundingBoxComponent(const glm::vec4& color) : data{std::make_unique<BoundingBoxData>()}, color{color} {}

    void update(const std::pair<glm::vec3, glm::vec3>& limits)
    {
        const auto& [lower, upper] = limits;
        data->vertices[0] = {lower.x, lower.y, lower.z};
        data->vertices[1] = {upper.x, lower.y, lower.z};
        data->vertices[2] = {upper.x, upper.y, lower.z};
        data->vertices[3] = {lower.x, upper.y, lower.z};
        data->vertices[4] = {lower.x, lower.y, upper.z};
        data->vertices[5] = {upper.x, lower.y, upper.z};
        data->vertices[6] = {upper.x, upper.y, upper.z};
        data->vertices[7] = {lower.x, upper.y, upper.z};
    }
};

//------------------------------------------------------
//                      Triangulation
//------------------------------------------------------

struct TriangulationData
{
    std::vector<glm::vec3> vertices;
    std::vector<std::array<uint16_t, 3>> indices;
};

struct TriangulationComponent
{
    std::unique_ptr<TriangulationData> data;
    std::pair<glm::vec3, glm::vec3> limits;

    TriangulationComponent() : data{std::make_unique<TriangulationData>()} {}
    TriangulationComponent(const TriangulationComponent&) = default;

    void update(const glm::mat4& transform)
    {
        auto& [lower, upper] = limits;
        lower.x = lower.y = lower.z = std::numeric_limits<float>::max();
        upper.x = upper.y = upper.z = std::numeric_limits<float>::min();
        for (auto& vertex : data->vertices) {
            vertex = glm::vec4(vertex, 1.0f) * transform;
            lower.x = std::min(lower.x, vertex.x);
            lower.y = std::min(lower.y, vertex.y);
            lower.z = std::min(lower.z, vertex.z);
            upper.x = std::max(upper.x, vertex.x);
            upper.y = std::max(upper.y, vertex.y);
            upper.z = std::max(upper.z, vertex.z);
        }
    }
};

//------------------------------------------------------
//                      Plane
//------------------------------------------------------

class Texture2D;
struct PlaneRendererComponent
{
    std::variant<glm::vec4, std::shared_ptr<Texture2D>> material;

    PlaneRendererComponent() : material{glm::vec4(1.0f)} {}
    PlaneRendererComponent(const PlaneRendererComponent&) = default;
    PlaneRendererComponent(const glm::vec4& color) : material{color} {}
    PlaneRendererComponent(const std::shared_ptr<Texture2D>& texture) : material{texture} {}

    inline bool hasTexture() const { return material.index() == 1; }

    static std::unique_ptr<TriangulationData> createTriangulation()
    {
        auto triangulation = std::make_unique<TriangulationData>();

        triangulation->vertices.resize(PlaneData::vertices.size());
        for (size_t i = 0; i < PlaneData::vertices.size(); ++i) {
            triangulation->vertices[i].x = PlaneData::vertices[i].position.x;
            triangulation->vertices[i].y = PlaneData::vertices[i].position.y;
            triangulation->vertices[i].z = PlaneData::vertices[i].position.z;
        }
        triangulation->indices.resize(PlaneData::indices.size());
        for (size_t i = 0; i < PlaneData::indices.size(); ++i)
            triangulation->indices[i] = PlaneData::indices[i];
            
        return triangulation;
    }
};

//------------------------------------------------------
//                      Frame
//------------------------------------------------------

struct FrameRendererComponent
{
    bool placeholder;

    FrameRendererComponent() = default;
    FrameRendererComponent(const FrameRendererComponent&) = default;

    static std::unique_ptr<TriangulationData> createTriangulation()
    {
        auto triangulation = std::make_unique<TriangulationData>();

        triangulation->vertices.resize(FrameData::vertices.size());
        for (size_t i = 0; i < FrameData::vertices.size(); ++i) {
            triangulation->vertices[i].x = FrameData::vertices[i].position.x;
            triangulation->vertices[i].y = FrameData::vertices[i].position.y;
            triangulation->vertices[i].z = FrameData::vertices[i].position.z;
        }
        triangulation->indices.resize(FrameData::indices.size());
        for (size_t i = 0; i < FrameData::indices.size(); ++i)
            triangulation->indices[i] = FrameData::indices[i];

        return triangulation;
    }
};

//------------------------------------------------------
//                      Sphere
//------------------------------------------------------

struct SphereRendererComponent
{
    glm::vec4 color;

    SphereRendererComponent() : color(1.0f) {}
    SphereRendererComponent(const SphereRendererComponent&) = default;
    SphereRendererComponent(const glm::vec4& color) : color{color} {}

    static std::unique_ptr<TriangulationData> createTriangulation()
    {
        auto triangulation = std::make_unique<TriangulationData>();

        const auto [vertices, indices] = SphereData::generateSphere();
        triangulation->vertices.resize(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            triangulation->vertices[i].x = vertices[i].position.x;
            triangulation->vertices[i].y = vertices[i].position.y;
            triangulation->vertices[i].z = vertices[i].position.z;
        }
        triangulation->indices.resize(indices.size());
        for (size_t i = 0; i < indices.size(); ++i)
            triangulation->indices[i] = indices[i];

        return triangulation;
    }
};

//------------------------------------------------------
//                      Mesh
//------------------------------------------------------

struct MeshRendererComponent
{
    UUID id;

    MeshRendererComponent(const UUID id) : id{id} {}
    MeshRendererComponent() = delete;
    MeshRendererComponent(const MeshRendererComponent&) = default;
};