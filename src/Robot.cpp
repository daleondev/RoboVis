#include "pch.h"

#include "Robot.h"
#include "Scene.h"

#include "Xml/XmlParser.h"

#include "Util/geometry.h"
#include "Util/util.h"

Robot::Robot() = default;

Robot::~Robot() = default;

bool Robot::setup(const std::string& sourceDir)
{
    // const auto origPath = std::filesystem::current_path();

    const auto robDir = std::filesystem::path(sourceDir);
    if (!std::filesystem::exists(robDir)) {
        std::cerr << "Robot model directory invalid." << std::endl;
		return false;
    }

    const auto urdfDir = std::filesystem::path(sourceDir + "/urdf");
    auto meshDir = std::filesystem::path(sourceDir + "/meshes");
    if (!std::filesystem::exists(urdfDir) || !std::filesystem::exists(meshDir)) {
        std::cerr << "Robot model directory has invalid folder structure." << std::endl;
		return false;
    }

    std::string urdfContent;
    for (const auto& entry : std::filesystem::directory_iterator(urdfDir)) {
        if (entry.path().extension() == ".urdf") {
            urdfContent = readFile(entry.path());
            break;
        }
    }
    if (urdfContent.empty()) {
        std::cerr << "Robot model directory does not contain valid urdf-data." << std::endl;
		return false;
    }

    XmlLexer urdfLexer(urdfContent);
    XmlParser urdfParser(urdfLexer.generateTokens());

    // Root
    const XmlNode urdfRoot = urdfParser.parse();
    if (urdfRoot.children.size() != 1 || urdfRoot.children.front().tag != "robot") {
        std::cerr << "Invalid urdf format." << std::endl;
		return false;
    }

    // Robot
    const XmlNode robotNode = urdfRoot.children.front();
    if (auto it = robotNode.attributes.find("name"); it == robotNode.attributes.cend() || it->second.index() != 2) {
        std::cerr << "Invalid robot tag." << std::endl;
		return false;
    }
    m_name = std::get<std::string>(robotNode.attributes.at("name"));
    std::cout << "Robot: " << m_name << std::endl;
    
    // Links/Joints
    for (const auto& node : robotNode.children) {
        if (auto it = node.attributes.find("name"); it != node.attributes.cend() && it->second.index() == 2) {
            const std::string name = std::get<std::string>(it->second);

            // Link
            if (node.tag == "link") {

                // extract node with visual data
                std::optional<XmlNode> visualNode;
                for (const auto& child : node.children) {
                    if (child.tag == "visual")
                        visualNode = child;
                    else if (!visualNode && child.tag == "collision")
                        visualNode = child;
                }
                if (!visualNode) {
                    std::cerr << "\tVisual node missing: " << name << std::endl;
                    continue;
                }

                // extract node with transformation data and node with geometry data
                std::optional<XmlNode> originNode;
                std::optional<XmlNode> geometryNode;
                for (const auto& child : visualNode->children) {
                    if (child.tag == "origin")
                        originNode = child;
                    else if (child.tag == "geometry")
                        geometryNode = child;
                        
                    if (originNode && geometryNode)
                        break;
                }

                // ------------- Transformation

                // check transformation data
                if (!originNode) {
                    std::cerr << "\tOrigin node missing: " << name << std::endl;
                    return false;
                }

                // get translation
                it = originNode->attributes.find("xyz");
                glm::vec3 p_mesh_world(0.0f);
                if (it != originNode->attributes.cend() && it->second.index() == 2)
                    p_mesh_world = strToVec3(std::get<std::string>(it->second));
                else
                    std::cerr << "\tNo xyz specified: " << name << std::endl;

                // get rotation
                it = originNode->attributes.find("rpy");
                glm::vec3 r_mesh_world(0.0f);
                if (it != originNode->attributes.cend() && it->second.index() == 2)
                    r_mesh_world = strToVec3(std::get<std::string>(it->second));
                else
                    std::cerr << "\tNo rpy specified: " << name << std::endl;

                glm::mat4 t_mesh_world =    angleAxisF(r_mesh_world.r, glm::vec3(1, 0, 0)) * 
                                            angleAxisF(r_mesh_world.p, glm::vec3(0, 1, 0)) * 
                                            angleAxisF(r_mesh_world.y, glm::vec3(0, 0, 1));
                setMat4Translation(t_mesh_world, p_mesh_world);

                // ------------- Mesh

                // check geometry data
                if (!geometryNode) {
                    std::cerr << "\tGeometry node missing: " << name << std::endl;
                    return false;
                }

                // extract node with geometry data
                std::optional<XmlNode> meshNode;
                for (const auto& child : geometryNode->children) 
                    if (child.tag == "mesh") {
                        meshNode = child;
                        break;
                    }
                if (!meshNode) {
                    std::cerr << "\tMesh node missing: " << name << std::endl;
                    return false;
                }

                // get mesh file name
                it = meshNode->attributes.find("filename");
                if (it == meshNode->attributes.cend() || it->second.index() != 2) {
                    std::cerr << "\tNo mesh-file specified: " << name << std::endl;
                    return false;
                }

                // check mesh file
                const std::filesystem::path meshFile = meshDir.string() + '/' + std::get<std::string>(it->second);
                if (aiIsExtensionSupported(meshFile.extension().c_str()) == AI_FALSE) {
                    std::cerr << "\tInvalid mesh-file: " << name << std::endl;
                    return false;
                }      

                // create entity with mesh data
                const aiScene* meshSource = aiImportFile(meshFile.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
                const auto mesh = Scene::createMesh(name, meshSource, t_mesh_world);
                aiReleaseImport(meshSource);

                // add link
                std::cout << "\tadding link: " << name << std::endl;
                m_links.emplace(name, std::make_shared<LinkData>(name, mesh));

                return true;
            }

            // Joint
            else if (node.tag == "joint") {
                

                std::cout << "\tadding joint: " << name << std::endl;
            }
        }
    }

    return true;
}