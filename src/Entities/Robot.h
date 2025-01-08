#pragma once

#include "Renderer/Camera.h"

#include "Entity.h"

#include "Xml/XmlParser.h"

#include "Util/EdgeDetector.h"

class Mesh;
class Frame;

struct LinkData
{
    std::string name;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Frame> frame;
};

struct JointData
{
    std::string name;  
    std::shared_ptr<LinkData> parent;
    std::shared_ptr<LinkData> child;
    glm::mat4 parentToChild;
    glm::vec3 rotationAxis;
    std::pair<float, float> limits;
};

struct Trajectory
{
    bool active = false;
    float currentTime = 0.0f;
    size_t currentIndex = 0;
    std::vector<std::vector<float>> jointValues;
    std::vector<float> times;
};

struct RobotControlData
{
    std::vector<float> jointValues;
    glm::mat4 transformation;
    bool drawFrames;
    bool drawBoundingBoxes;
    std::optional<Trajectory> trajectory;
};

class Robot : public Entity
{
public:
    Robot();
    ~Robot();

    bool setup(const std::filesystem::path& sourceDir);

    void update(const Timestep dt);
    
    virtual void draw(const Camera& camera) override;
    virtual void updateTriangulationData() override;

    virtual bool rayIntersection(const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist) const;

    void loadTrajectory(const std::filesystem::path& file);

    inline const std::string& getName() const { return m_name; }

    inline RobotControlData& getControlData() { return m_controlData; }
    inline const std::unordered_map<std::string, std::shared_ptr<LinkData>>& getLinks() const { return m_links; }
    inline const std::vector<std::shared_ptr<JointData>>& getJoints() const { return m_joints; }
    inline size_t numLinks() const { return m_links.size(); }
    inline size_t numJoints() const { return m_joints.size(); }

private:
    bool setupLink(const std::string& name, const std::filesystem::path& meshDir, const XmlNode& linkNode);
    bool setupJoint(const std::string& name, const XmlNode& jointNode);

    void addEntity(const std::string& name, const std::shared_ptr<Entity>& entity);

    glm::mat4 forwardTransform();

    std::string m_name;
    std::unordered_map<std::string, std::shared_ptr<Entity>> m_entities;

    std::unordered_map<std::string, std::shared_ptr<LinkData>> m_links;
    std::vector<std::shared_ptr<JointData>> m_joints;

    RobotControlData m_controlData;

};