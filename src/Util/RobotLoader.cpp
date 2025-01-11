#include "pch.h"

#include "RobotLoader.h"
#include "MeshLoader.h"

#include "Scene/Scene.h"
#include "Scene/Components.h"

#include "Renderer/Renderer.h"

#include "Util/Log.h"

Entity RobotLoader::loadRobot(const std::filesystem::path& sourceDir)
{
    if (!std::filesystem::exists(sourceDir)) {
        LOG_ERROR << "Robot model directory invalid.";
		return static_cast<entt::entity>(entt::null);
    }

    const auto urdfDir = sourceDir / "urdf";
    auto meshDir = sourceDir / "meshes";
    if (!std::filesystem::exists(urdfDir) || !std::filesystem::exists(meshDir)) {
        LOG_ERROR << "Robot model directory has invalid folder structure.";
		return static_cast<entt::entity>(entt::null);
    }

    std::string urdfContent;
    for (const auto& entry : std::filesystem::directory_iterator(urdfDir)) {
        if (entry.path().extension() == ".urdf") {
            urdfContent = readFile(entry.path());
            break;
        }
    }
    if (urdfContent.empty()) {
        LOG_ERROR << "Robot model directory does not contain valid urdf-data.";
		return static_cast<entt::entity>(entt::null);
    }

    // parse urdf file
    XmlLexer urdfLexer(urdfContent);
    XmlParser urdfParser(urdfLexer.generateTokens());

    // root
    const XmlNode urdfRoot = urdfParser.parse();
    if (urdfRoot.children.size() != 1 || urdfRoot.children.front().tag != "robot") {
        LOG_ERROR << "Invalid urdf format.";
		return static_cast<entt::entity>(entt::null);
    }

    // robot
    const XmlNode robotNode = urdfRoot.children.front();
    if (auto it = robotNode.attributes.find("name"); it == robotNode.attributes.cend() || it->second.index() != 2) {
        LOG_ERROR << "Invalid robot tag.";
		return static_cast<entt::entity>(entt::null);
    }

    const auto name = std::get<std::string>(robotNode.attributes.at("name"));
    LOG_INFO << "adding Robot: " << name;
    
    // links/joints
    s_links.clear();
    for (const auto& node : robotNode.children) {
        if (auto it = node.attributes.find("name"); it != node.attributes.cend() && it->second.index() == 2) {
            const std::string name = std::get<std::string>(it->second);

            // link
            if (node.tag == "link") {

                if(!setupLink(name, meshDir, node)) {
                    for (auto [name, entity] : s_links) {
                        Scene::destroyEntity(entity);
                    }
                    return static_cast<entt::entity>(entt::null);
                }

                // return true;
            }
            // joint
            else if (node.tag == "joint") {

                if(!setupJoint(name, node)) {
                    for (auto [name, entity] : s_links) {
                        Scene::destroyEntity(entity);
                    }
                    return static_cast<entt::entity>(entt::null);
                }
                
            }
        }
    }

    // find base link
    std::vector<std::string> linksWithChild;
    for (const auto& [name, entity] : s_links) {
        if (entity.hasComponent<JointComponent>())
            if (auto child = entity.getComponent<JointComponent>().childLink; child)
                linksWithChild.push_back(child.getTag());
    }
    auto baseLink = std::find_if(s_links.begin(), s_links.end(), [&linksWithChild](const auto& link) -> bool {
        return (std::ranges::find(linksWithChild, link.first) == linksWithChild.end()) && ((Entity)link.second).hasComponent<JointComponent>();
    });
    if (baseLink == s_links.end()) {
        LOG_ERROR << "No valid base link found";
        for (auto [name, entity] : s_links) {
            Scene::destroyEntity(entity);
        }
        return static_cast<entt::entity>(entt::null);
    }

    // create robot entity
    Entity robot = Scene::createEntity(name);
    robot.addComponent<VisibilityComponent>();
    robot.addComponent<TransformComponent>();
    auto& robotComponent = robot.addComponent<RobotComponent>(baseLink->second);
    robotComponent.drawFrames = true;
    robotComponent.drawBoundingBoxes = false;

    return robot;
}

bool RobotLoader::setupLink(const std::string& name, const std::filesystem::path& meshDir, const XmlNode& linkNode)
{
    // extract node with visual data
    std::optional<XmlNode> visualNode;
    for (const auto& child : linkNode.children) {
        if (child.tag == "visual")
            visualNode = child;
        else if (!visualNode && child.tag == "collision")
            visualNode = child;
    }
    if (!visualNode) {
        LOG_WARN << "Visual node missing: " << name;
        return true;
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

    glm::mat4 t_mesh_world(1.0f);
    if (originNode) {
        // get translation
        auto it = originNode->attributes.find("xyz");
        glm::vec3 p_mesh_world(0.0f);
        if (it != originNode->attributes.cend() && it->second.index() == 2)
            p_mesh_world = 1000.0f*strToVec3(std::get<std::string>(it->second));
        else
            LOG_WARN << "No xyz specified: " << name;

        // get rotation
        it = originNode->attributes.find("rpy");
        glm::vec3 r_mesh_world(0.0f);
        if (it != originNode->attributes.cend() && it->second.index() == 2)
            r_mesh_world = strToVec3(std::get<std::string>(it->second));
        else
            LOG_WARN << "No rpy specified: " << name;

        t_mesh_world = eulerXYZ(r_mesh_world);
        setMat4Translation(t_mesh_world, p_mesh_world);
    }
    else
        LOG_WARN << "Origin node missing: " << name;

    // ------------- Mesh

    // check geometry data
    if (!geometryNode) {
        LOG_WARN << "Geometry node missing: " << name;
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
        LOG_ERROR << "Mesh node missing: " << name;
        return false;
    }

    // get mesh file name
    auto it = meshNode->attributes.find("filename");
    if (it == meshNode->attributes.cend() || it->second.index() != 2) {
        LOG_ERROR << "No mesh-file specified: " << name;
        return false;
    }

    // load mesh file
    const std::filesystem::path meshFile = meshDir.string() + '/' + std::get<std::string>(it->second);
    if (aiIsExtensionSupported(meshFile.extension().c_str()) == AI_FALSE) {
        LOG_ERROR << "Invalid mesh-file: " << name;
        return false;
    }      
    auto mesh = MeshLibrary::load(meshFile, t_mesh_world);

    // create link entity
    LOG_INFO << "creating link: " << name;
    auto link = Scene::createEntity(name);
    link.addComponent<MeshRendererComponent>(mesh->id);
    link.addComponent<FrameRendererComponent>();

    // triangulation
    auto& triangulation = link.getComponent<TriangulationComponent>();
    for (auto& vertex : mesh->data.vertices)
        triangulation.data->vertices.emplace_back(vertex.position);
    triangulation.data->indices = mesh->data.indices;

    // bounding box
    auto& boundingBox = link.getComponent<BoundingBoxComponent>();
    triangulation.update(glm::mat4(1.0f));
    boundingBox.update(triangulation.limits);

    // render data
    Renderer::addMeshData(mesh->id, mesh->data);
    Renderer::addBoxData(mesh->id, BoxData{boundingBox.data->vertices});

    s_links.emplace(name, link);
    return true;
}

bool RobotLoader::setupJoint(const std::string& name, const XmlNode& linkNode)
{
    // extract nodes with transformation data, parent, child and limit
    std::optional<XmlNode> originNode, parentNode, childNode, axisNode, limitNode;
    for (const auto& child : linkNode.children) {
        if (child.tag == "origin")
            originNode = child;
        else if (child.tag == "parent")
            parentNode = child;
        else if (child.tag == "child")
            childNode = child;
        else if (child.tag == "axis")
            axisNode = child;
        else if (child.tag == "limit")
            limitNode = child;
    }

    // ------------- Transformation

    if (!originNode) {
        LOG_ERROR << "Origin node missing: " << name;
        return false;
    }
    
    // get translation
    auto it = originNode->attributes.find("xyz");
    glm::vec3 p_child_parent(0.0f);
    if (it != originNode->attributes.cend() && it->second.index() == 2)
        p_child_parent = 1000.0f*strToVec3(std::get<std::string>(it->second));
    else
        LOG_WARN << "No xyz specified: " << name;

    // get rotation
    it = originNode->attributes.find("rpy");
    glm::vec3 r_child_parent(0.0f);
    if (it != originNode->attributes.cend() && it->second.index() == 2)
        r_child_parent = strToVec3(std::get<std::string>(it->second));
    else
        LOG_WARN << "No rpy specified: " << name;

    glm::mat4 t_child_parent = eulerXYZ(r_child_parent);
    setMat4Translation(t_child_parent, p_child_parent);

    // ------------- Parent/Child

    // get parent
    if (!parentNode) {
        LOG_ERROR << "Parent node missing: " << name;
        return false;
    }

    entt::entity e = static_cast<entt::entity>(entt::null);
    it = parentNode->attributes.find("link");
    if (it != parentNode->attributes.cend() && it->second.index() == 2) {
        const auto parentName = std::get<std::string>(it->second);
        if (const auto p = s_links.find(parentName); p != s_links.end())
            e = p->second;
    }

    Entity parent(e);
    if (!parent) {
        LOG_WARN << "No valid parent specified: " << name;
        return true;
    }

    // get child
    if (!childNode) {
        LOG_ERROR << "Parent node missing: " << name;
        return false;
    }

    it = childNode->attributes.find("link");
    e = static_cast<entt::entity>(entt::null);
    if (it != childNode->attributes.cend() && it->second.index() == 2) {
        const auto childName = std::get<std::string>(it->second);
        if (const auto p = s_links.find(childName); p != s_links.end())
            e = p->second;
    }

    Entity child(e);
    if (!child) {
        LOG_WARN << "No valid child specified: " << name;
        return true;
    }

    // ------------- Axis

    // get axis
    glm::vec3 axis(1.0f, 0.0f, 0.0f);
    if (axisNode) {
        it = axisNode->attributes.find("xyz");
        if (it != limitNode->attributes.cend() && it->second.index() == 2)
            axis = glm::normalize(strToVec3(std::get<std::string>(it->second)));
        else {
            LOG_ERROR << "Axis xyz missing: " << name;
            return false;
        }
    }
    else 
        LOG_WARN << "Axis node missing: " << name;

    // ------------- Limits

    // get limits
    glm::vec2 limits{0.0f, 0.0f};
    if (limitNode) {
        it = limitNode->attributes.find("lower");
        if (it != limitNode->attributes.cend() && it->second.index() == 1)
            limits[0] = std::get<float>(it->second);
        else
            LOG_WARN << "No lower limit specified: " << name;
            
        it = limitNode->attributes.find("upper");
        if (it != limitNode->attributes.cend() && it->second.index() == 1)
            limits[1] = std::get<float>(it->second);
        else
            LOG_WARN << "No upper limit specified: " << name;
    }
    else
        LOG_WARN << "No limits specified: " << name;

    // create joint entity
    LOG_INFO << "creating joint: " << name << ", parent = " << parent.getComponent<TagComponent>().tag;
    auto& joint = parent.addComponent<JointComponent>(name, parent, child, t_child_parent, axis, limits);
    joint.value = (limits[0]+limits[1])/2.0f;

    return true;
}