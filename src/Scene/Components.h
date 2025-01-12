#pragma once

#include "Entity.h"

#include "Renderer/RenderData.h"

#include "Util/util.h"
#include "Util/geometry.h"
#include "Util/EdgeDetector.h"

//------------------------------------------------------
//                  Identification
//------------------------------------------------------

struct IdComponent
{
    const UUID id;

    IdComponent() : id{uuid()} {}
    IdComponent(const IdComponent&) = default;

    operator UUID() const { return id; }
};

struct TagComponent
{
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : tag{tag} {}

    operator std::string() const { return tag; }
};

//------------------------------------------------------
//                    Transform
//------------------------------------------------------

struct TransformComponent;
void setTransform(TransformComponent& component, const glm::mat4& transform);
glm::mat4 getTransform(const TransformComponent& component);

struct TransformComponent
{
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;

    TransformComponent() : translation{0.0f}, rotation{0.0f}, scale{1.0f} {};
    TransformComponent(const TransformComponent&) = default;

    operator glm::mat4() const { return getTransform(*this); }

private:
    EdgeDetector<glm::vec3> translationListener;
    EdgeDetector<glm::vec3> rotationListener;
    EdgeDetector<glm::vec3> scaleListener;
    std::optional<std::function<void(const glm::mat4&)>> changedCallback;

    friend void setTransformChangedCallback(TransformComponent& component, const std::function<void(const glm::mat4&)>& callback);
    friend void detectTransformChange(TransformComponent& component);
};

//------------------------------------------------------
//                      Properties
//------------------------------------------------------

struct PropertiesComponent
{
    bool visible;
    bool editable;
    bool removable;
    bool copyable;
    bool clickable;
    
    PropertiesComponent() : visible{true}, removable{true}, editable{true}, copyable{true}, clickable{true} {}
    PropertiesComponent(const PropertiesComponent&) = default;
};

//------------------------------------------------------
//                      Bounding Box
//------------------------------------------------------

struct BoundingBoxData
{
    std::array<glm::vec3, 8> vertices;
    static constexpr std::array<std::array<uint32_t, 3>, 12> indices = BoxData::indices;
};

struct BoundingBoxComponent
{
    bool visible = false;
    std::shared_ptr<BoundingBoxData> data;
    glm::vec4 color;

    BoundingBoxComponent() : data{std::make_shared<BoundingBoxData>()}, color(1.0f) {}
    BoundingBoxComponent(BoundingBoxComponent& other) = default;
    BoundingBoxComponent(const glm::vec4& color) : data{std::make_shared<BoundingBoxData>()}, color{color} {}
};

//------------------------------------------------------
//                      Triangulation
//------------------------------------------------------

struct TriangulationComponent;
void updateTriangulation(TriangulationComponent&, const glm::mat4&);

struct TriangulationData
{
    std::vector<glm::vec3> vertices;
    std::vector<std::array<uint32_t, 3>> indices;
};

struct TriangulationComponent
{
    std::shared_ptr<TriangulationData> data;
    std::vector<glm::vec3> updatedVertices;
    std::pair<glm::vec3, glm::vec3> limits;

    TriangulationComponent() : data{std::make_shared<TriangulationData>()} {}
    TriangulationComponent(const TriangulationComponent&) = default;
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

    static std::shared_ptr<TriangulationData> createTriangulation()
    {
        auto triangulation = std::make_shared<TriangulationData>();

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
    std::optional<glm::mat4> internalTransform;
    bool internalVisible;

    FrameRendererComponent() = default;
    FrameRendererComponent(const FrameRendererComponent&) = default;

    static std::shared_ptr<TriangulationData> createTriangulation()
    {
        auto triangulation = std::make_shared<TriangulationData>();

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

    static std::shared_ptr<TriangulationData> createTriangulation()
    {
        auto triangulation = std::make_shared<TriangulationData>();

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

    MeshRendererComponent() = default;
    MeshRendererComponent(const MeshRendererComponent&) = default;
    MeshRendererComponent(const UUID id) : id{id} {}
};

//------------------------------------------------------
//                      Joint
//------------------------------------------------------

enum class JointType
{
    Invalid,
    Fixed,
    Revolute
};

struct JointComponent
{
    std::string name;
    JointType type;
    Entity parentLink;
    Entity childLink;
    glm::mat4 parentToChild;
    glm::vec3 rotationAxis;
    glm::vec2 limits;
    float value;

    JointComponent() = default;
    JointComponent(const JointComponent&) = default;
    JointComponent(const std::string& name, const JointType type, const Entity parent, const Entity child, const glm::mat4& parentToChild, const glm::vec3& rotationAxis, const glm::vec2& limits)
        : name{name}, type{type}, parentLink{parent}, childLink{child}, parentToChild{parentToChild}, rotationAxis{rotationAxis}, limits{limits}, value{(limits[0]+limits[1])/2.0f} {}
};

//------------------------------------------------------
//                      Robot
//------------------------------------------------------

struct RobotComponent;
void setupRobot(RobotComponent&);

struct Trajectory
{
    bool active = false;
    float currentTime = 0.0f;
    size_t currentIndex = 0;
    std::vector<std::vector<float>> jointValues;
    std::vector<float> times;
};

struct RobotComponent
{
    Entity baseLink;

    EdgeDetector<bool> visible;
    EdgeDetector<bool> clickable;
    EdgeDetector<bool> drawFrames;
    EdgeDetector<float> frameScale;
    EdgeDetector<bool> drawBoundingBoxes;
    EdgeDetector<glm::vec4> boundingBoxColor;

    std::vector<std::reference_wrapper<JointComponent>> joints;
    std::optional<Trajectory> trajectory;

    RobotComponent() = default; 
    RobotComponent(const RobotComponent&) = default;
    RobotComponent(const Entity baseLink) : baseLink{baseLink}, visible{true}, clickable{true}, drawFrames{true}, frameScale{0.4f},
                                            drawBoundingBoxes{false}, boundingBoxColor{glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}} { setupRobot(*this); }
};