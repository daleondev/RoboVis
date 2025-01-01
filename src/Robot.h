#pragma once

#include "Entities/Mesh.h"

#include "Xml/XmlParser.h"

struct LinkData
{
    std::string name;
    std::shared_ptr<Mesh> mesh;
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

class Robot
{
public:
    Robot();
    ~Robot();

    bool setup(const std::string& sourceDir);

private:
    bool setupLink(const std::string& name, const std::filesystem::path& meshDir, const XmlNode& linkNode);
    bool setupJoint(const std::string& name, const XmlNode& jointNode);

    std::string m_name;
    
    std::unordered_map<std::string, std::shared_ptr<LinkData>> m_links;
    std::unordered_map<std::string, std::shared_ptr<JointData>> m_joints;
    std::vector<float> m_jointValues;

};