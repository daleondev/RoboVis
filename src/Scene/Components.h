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
        glm::mat4 transform(1.0f);
        setMat4Translation(transform, translation);
        setMat4Rotation(transform, eulerZYX(rotation));
        return glm::transpose(glm::scale(glm::transpose(transform), scale));
    }
    operator glm::mat4() const { return get(); }

    inline void setChangedCallback(const std::function<void(const glm::mat4&)>& callback) { changedCallback = callback; }
    void detectChange() 
    { 
        (glm::vec3&)translationListener = translation;
        (glm::vec3&)rotationListener = rotation;
        (glm::vec3&)scaleListener = scale;
        if (changedCallback && (translationListener().edge() || rotationListener().edge() || scaleListener().edge())) 
            (*changedCallback)(get()); 
    }

private:
    EdgeDetector<glm::vec3> translationListener;
    EdgeDetector<glm::vec3> rotationListener;
    EdgeDetector<glm::vec3> scaleListener;
    std::optional<std::function<void(const glm::mat4&)>> changedCallback;

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

    bool rayIntersects(const std::tuple<glm::vec3, glm::vec3>& ray_world) const
    {
        const auto& [v_ray_world, p_ray_world] = ray_world;

        bool hit = false;
        for (const auto& indices : data->indices) {
            std::array<glm::vec3, 3> p_vertices_world;
            for (size_t i = 0; i < 3; ++i)
                p_vertices_world[i] = data->vertices[indices[i]];

            const auto[n_tri_world, p_tri_world] = trianglePlane(p_vertices_world);
            glm::vec3 p_hitTmp_world;                               
            if (intersectionLinePlane(n_tri_world, p_tri_world, v_ray_world, p_ray_world, p_hitTmp_world) && 
                pointInTriangle(n_tri_world, p_tri_world, p_vertices_world, p_hitTmp_world)) {   

                hit = true;
                break;
            }
        }

        return hit;
    }
};

//------------------------------------------------------
//                      Triangulation
//------------------------------------------------------

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

    void update(const glm::mat4& transform)
    {
        auto& [lower, upper] = limits;
        lower.x = lower.y = lower.z = std::numeric_limits<float>::max();
        upper.x = upper.y = upper.z = std::numeric_limits<float>::min();

        if (updatedVertices.size() != data->vertices.size()) 
            updatedVertices.resize(data->vertices.size());
            
        for (size_t i = 0; i < updatedVertices.size(); ++i) {
            updatedVertices[i] = glm::vec4(data->vertices[i], 1.0f) * transform;
            lower.x = std::min(lower.x, updatedVertices[i].x);
            lower.y = std::min(lower.y, updatedVertices[i].y);
            lower.z = std::min(lower.z, updatedVertices[i].z);
            upper.x = std::max(upper.x, updatedVertices[i].x);
            upper.y = std::max(upper.y, updatedVertices[i].y);
            upper.z = std::max(upper.z, updatedVertices[i].z);      
        }
    }

    bool rayIntersection(const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist) const
    {
        const auto& [v_ray_world, p_ray_world] = ray_world;

        minDist = std::numeric_limits<float>::max();
        bool hit = false;
        for (const auto& indices : data->indices) {
            std::array<glm::vec3, 3> p_vertices_world;
            for (size_t i = 0; i < 3; ++i)
                p_vertices_world[i] = updatedVertices[indices[i]];

            const auto[n_tri_world, p_tri_world] = trianglePlane(p_vertices_world);
            glm::vec3 p_hitTmp_world;                               
            if (intersectionLinePlane(n_tri_world, p_tri_world, v_ray_world, p_ray_world, p_hitTmp_world) && 
                pointInTriangle(n_tri_world, p_tri_world, p_vertices_world, p_hitTmp_world)) {  

                const float dist = glm::length(p_hitTmp_world - p_ray_world);
                if (!hit || dist < minDist) {
                    hit = true;
                    p_hit_world = p_hitTmp_world;
                    minDist = dist;
                }
            }
        }

        return hit;
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
//                      Robot
//------------------------------------------------------

struct JointComponent
{
    enum class Type
    {
        Invalid,
        Fixed,
        Revolute
    };

    const std::string name;
    const Type type;
    const Entity parentLink;
    const Entity childLink;
    const glm::mat4 parentToChild;
    const glm::vec3 rotationAxis;
    const glm::vec2 limits;

    float value;

    JointComponent(const std::string& name, const Type type, const Entity parent, const Entity child, const glm::mat4& parentToChild, const glm::vec3& rotationAxis, const glm::vec2& limits)
        : name{name}, type{type}, parentLink{parent}, childLink{child}, parentToChild{parentToChild}, rotationAxis{rotationAxis}, limits{limits}, value{(limits[0]+limits[1])/2.0f} {}
    JointComponent() = delete;
    JointComponent(const JointComponent&) = default;

    glm::mat4 forwardTransform()
    {
        value = std::clamp(value, limits[0], limits[1]);
        const auto t_parent_world = parentLink.getComponent<TransformComponent>().get();
        const auto t_child_world = glm::mat4(angleAxisF(value, rotationAxis)) * parentToChild * t_parent_world;
        childLink.getComponent<TransformComponent>().set(t_child_world);

        if (childLink.hasComponent<JointComponent>())
            return childLink.getComponent<JointComponent>().forwardTransform();
        else
            return t_child_world;
    }

    void destroy(const bool destroyParent = true)
    {
        if (childLink.hasComponent<JointComponent>())
            childLink.getComponent<JointComponent>().destroy(false);
        Scene::destroyEntity(childLink);
        if (destroyParent)
            Scene::destroyEntity(parentLink);
    }

    void recurse(const std::function<void(Entity)>& func, const bool execForParent = true)
    {
        if (childLink.hasComponent<JointComponent>())
            childLink.getComponent<JointComponent>().recurse(func, false);
        func(childLink);  
        if (execForParent)
            func(parentLink);      
    }

    static constexpr const char* typeToStr(const Type type)
    {
        switch (type) {
            case Type::Fixed: return "fixed";
            case Type::Revolute: return "revolute";
            default: return "invalid";
        }
    }

    static constexpr Type strToType(const char* str)
    {
        if (const_strcmp(str, "fixed"))
            return Type::Fixed;
        else if (const_strcmp(str, "revolute"))
            return Type::Revolute;
        return Type::Invalid;
    }
};

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
    const Entity baseLink;

    EdgeDetector<bool> visible;
    EdgeDetector<bool> clickable;
    EdgeDetector<bool> drawFrames;
    EdgeDetector<float> frameScale;
    EdgeDetector<bool> drawBoundingBoxes;
    EdgeDetector<glm::vec4> boundingBoxColor;

    std::vector<std::reference_wrapper<JointComponent>> joints;
    std::optional<Trajectory> trajectory;

    RobotComponent(const Entity baseLink) : baseLink{baseLink}, visible{true}, clickable{true}, drawFrames{true}, frameScale{0.4f},
                                            drawBoundingBoxes{false}, boundingBoxColor{glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}} { setupJoints(); }
    RobotComponent() = delete;
    RobotComponent(const RobotComponent&) = default;

    void update(const Timestep dt, Entity entity)
    {
        if (entity.hasComponent<JointComponent>()) {
            auto& joint = entity.getComponent<JointComponent>();
            joint.forwardTransform();

            // properties
            auto& properties = entity.getComponent<PropertiesComponent>();
            if ((bool&)visible = properties.visible; visible().edge()) {
                joint.recurse([this](Entity entity) -> void {
                    if (entity.hasComponent<PropertiesComponent>())
                        entity.getComponent<PropertiesComponent>().visible = visible;
                });
            }
            if ((bool&)clickable = properties.clickable; clickable().edge()) {
                joint.recurse([this](Entity entity) -> void {
                    if (entity.hasComponent<PropertiesComponent>())
                        entity.getComponent<PropertiesComponent>().clickable = clickable;
                });
            }

            // frames
            if (drawFrames().edge()) {
                joint.recurse([this](Entity entity) -> void {
                    if (entity.hasComponent<FrameRendererComponent>())
                        entity.getComponent<FrameRendererComponent>().internalVisible = drawFrames;
                });
            }
            if (frameScale().edge()) {
                joint.recurse([this](Entity entity) -> void {
                    if (entity.hasComponent<FrameRendererComponent>() && entity.getComponent<FrameRendererComponent>().internalTransform) {
                        auto& t = *entity.getComponent<FrameRendererComponent>().internalTransform;
                        t = rescaleMat4(t) * glm::transpose(glm::scale(glm::transpose(glm::mat4(1.0f)), {frameScale, frameScale, frameScale}));
                        if (entity.hasComponent<TriangulationComponent>())
                            entity.getComponent<TriangulationComponent>().update(t);
                    }                   
                });
            }

            // bounding box
            if (drawBoundingBoxes().edge()) {
                joint.recurse([this](Entity entity) -> void {
                    if (entity.hasComponent<BoundingBoxComponent>())
                        entity.getComponent<BoundingBoxComponent>().visible = drawBoundingBoxes;
                });
            }
            if (boundingBoxColor().edge()) {
                joint.recurse([this](Entity entity) -> void {
                    if (entity.hasComponent<BoundingBoxComponent>())
                        entity.getComponent<BoundingBoxComponent>().color = boundingBoxColor;        
                });
            }

            // trajectory
            if (trajectory) {
                auto& [active, currentTime, currentIndex, jointValues, times] = *trajectory; 

                while (currentTime > times[currentIndex] && currentIndex < jointValues.size())
                    currentIndex++;
                while (currentTime < times[currentIndex-1] && currentIndex > 0)
                    currentIndex--;

                if (active && currentTime < times.back()) {
                    currentTime += dt;

                    if (currentIndex > 0 && currentIndex < jointValues.size())
                        for (size_t i = 0; i < joints.size(); ++i) {           
                            joints[joints.size()-i-1].get().value = map(
                                currentTime, 
                                times[currentIndex-1],          times[currentIndex], 
                                jointValues[currentIndex-1][i], jointValues[currentIndex][i]); 
                        } 
                }
                else if (active)
                    active = false;
            }
        }
    }

    void destroy()
    {
        if (baseLink.hasComponent<JointComponent>()) 
            baseLink.getComponent<JointComponent>().destroy();
        Scene::destroyEntity(baseLink);
    }

private:
    void setupJoints()
    {
        if (baseLink.hasComponent<JointComponent>()) {
            baseLink.getComponent<JointComponent>().recurse([this](Entity entity) -> void {
                if (entity.hasComponent<JointComponent>())
                    joints.push_back(entity.getComponent<JointComponent>());
            });
        }
    }

};