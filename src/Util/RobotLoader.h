#pragma once

#include "Scene/Entity.h"

#include "Xml/XmlParser.h"

class Trajectory;
class RobotLoader
{
public:
    static void init();

    static Entity loadRobot(const std::filesystem::path& sourceDir);
    static std::optional<Trajectory> loadTrajectory(const Entity entity, const std::filesystem::path& file);

private:
    static bool setupLink(const std::string& name, const std::filesystem::path& meshDir, const XmlNode& linkNode);
    static bool setupJoint(const std::string& name, const XmlNode& linkNode);

    inline static std::unordered_map<std::string, Entity> s_links = {};
};