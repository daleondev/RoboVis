#pragma once

#include "Entities/Mesh.h"

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
    std::string m_name;
    
    std::unordered_map<std::string, std::shared_ptr<LinkData>> m_links;
    std::unordered_map<std::string, std::shared_ptr<JointData>> m_joints;
    std::vector<float> m_jointValues;

};