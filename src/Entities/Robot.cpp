#include "pch.h"

#include "Robot.h"

#include "Mesh.h"
#include "Frame.h"

#include "ImGui/ImGuiLayer.h"

#include "Util/geometry.h"
#include "Util/Log.h"

Robot::Robot() = default;

Robot::~Robot() = default;

bool Robot::setup(const std::filesystem::path& sourceDir)
{
    if (!std::filesystem::exists(sourceDir)) {
        LOG_ERROR << "Robot model directory invalid.";
		return false;
    }

    const auto urdfDir = sourceDir / "urdf";
    auto meshDir = sourceDir / "meshes";
    if (!std::filesystem::exists(urdfDir) || !std::filesystem::exists(meshDir)) {
        LOG_ERROR << "Robot model directory has invalid folder structure.";
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
        LOG_ERROR << "Robot model directory does not contain valid urdf-data.";
		return false;
    }

    // parse urdf file
    XmlLexer urdfLexer(urdfContent);
    XmlParser urdfParser(urdfLexer.generateTokens());

    // root
    const XmlNode urdfRoot = urdfParser.parse();
    if (urdfRoot.children.size() != 1 || urdfRoot.children.front().tag != "robot") {
        LOG_ERROR << "Invalid urdf format.";
		return false;
    }

    // robot
    const XmlNode robotNode = urdfRoot.children.front();
    if (auto it = robotNode.attributes.find("name"); it == robotNode.attributes.cend() || it->second.index() != 2) {
        LOG_ERROR << "Invalid robot tag.";
		return false;
    }
    m_name = std::get<std::string>(robotNode.attributes.at("name"));
    LOG_INFO << "adding Robot: " << m_name;
    
    // links/joints
    for (const auto& node : robotNode.children) {
        if (auto it = node.attributes.find("name"); it != node.attributes.cend() && it->second.index() == 2) {
            const std::string name = std::get<std::string>(it->second);

            // link
            if (node.tag == "link") {

                if(!setupLink(name, meshDir, node))
                    return false;

                // return true;
            }
            // joint
            else if (node.tag == "joint") {

                if(!setupJoint(name, node))
                    return false;
                
            }
        }
    }

    m_controlData.drawFrames = true;
    m_controlData.drawBoundingBoxes = false;
    return true;
}

void Robot::update(const Timestep dt)
{
    if (m_controlData.trajectory) {
        auto& [active, currentTime, currentIndex, jointValues, times] = *m_controlData.trajectory; 

        while (currentTime > times[currentIndex] && currentIndex < jointValues.size())
            currentIndex++;
        while (currentTime < times[currentIndex-1] && currentIndex > 0)
            currentIndex--;

        if (active && m_controlData.trajectory->currentTime < m_controlData.trajectory->times.back()) {
            currentTime += dt;

            if (currentIndex > 0 && currentIndex < jointValues.size())
                for (size_t i = 0; i < numJoints(); ++i) {           
                    m_controlData.jointValues[i] = map(
                        currentTime, 
                        times[currentIndex-1],          times[currentIndex], 
                        jointValues[currentIndex-1][i], jointValues[currentIndex][i]); 
                } 
        }
        else if (active)
            active = false;
    }

    forwardTransform();
}

void Robot::draw(const Camera& camera)
{
    for (auto&[name, entity] : m_entities)
        if (auto mesh = dynamic_cast<Mesh*>(entity.get()); mesh != nullptr)
            mesh->draw(camera, m_controlData.drawBoundingBoxes);
        else
            entity->draw(camera);
}

void Robot::updateTriangulationData()
{
    for (auto&[name, entity] : m_entities)
        if (auto mesh = dynamic_cast<Mesh*>(entity.get()); mesh != nullptr)
            mesh->updateTriangulationData();
}

bool Robot::rayIntersection(const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist) const
{
    bool hit = false;
    glm::vec3 p_hitTmp_world;
    for (auto&[name, entity] : m_entities) {       
        float dist;

        if (entity->rayIntersection(ray_world, p_hitTmp_world, dist)) {
            if (!hit || dist < minDist) {
                hit = true;
                minDist = dist;
                p_hit_world = p_hitTmp_world;
            }
        }
    }

    return hit;
}

void Robot::loadTrajectory(const std::filesystem::path& file)
{
    std::ifstream in(file);
    if (!in) {
        LOG_ERROR << "Failed to open trajectory file: " << file;
        return;
    }

    Trajectory traj;
    std::string line;
    while (std::getline(in, line)) {
        const auto parts = splitString(line, ";, ");
        std::vector<float> jointValues(numJoints());
        for (size_t i = 0; i < numJoints(); ++i) {
            jointValues[i] = std::stof(parts[i]);
        }
        traj.jointValues.push_back(jointValues);
        traj.times.push_back(std::stof(parts[numJoints()]));
    }
    m_controlData.trajectory = traj;

    LOG_INFO << "Successfully loaded trajectory file: " << file;
}

bool Robot::setupLink(const std::string& name, const std::filesystem::path& meshDir, const XmlNode& linkNode)
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

    // check mesh file
    const std::filesystem::path meshFile = meshDir.string() + '/' + std::get<std::string>(it->second);
    if (aiIsExtensionSupported(meshFile.extension().c_str()) == AI_FALSE) {
        LOG_ERROR << "Invalid mesh-file: " << name;
        return false;
    }      

    // create entity with mesh data
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_COLLADA_IGNORE_UP_DIRECTION, 1);
    const aiScene* meshSource = importer.ReadFile(meshFile.c_str(), aiProcessPreset_TargetRealtime_Fast);
    const auto mesh = std::make_shared<Mesh>(meshSource, t_mesh_world);
    addEntity(name, mesh);
    importer.FreeScene();

    // create Frame
    const auto frame = std::make_shared<Frame>();
    addEntity("frame_" + name, frame);

    // add link
    LOG_INFO << "adding link: " << name;
    m_links.emplace(name, std::make_shared<LinkData>(name, mesh, frame));

    return true;
}

bool Robot::setupJoint(const std::string& name, const XmlNode& linkNode)
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

    it = parentNode->attributes.find("link");
    std::shared_ptr<LinkData> parent = nullptr;
    if (it != parentNode->attributes.cend() && it->second.index() == 2) {
        const auto parentName = std::get<std::string>(it->second);
        if (const auto p = m_links.find(parentName); p != m_links.end())
            parent = p->second;
    }

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
    std::shared_ptr<LinkData> child = nullptr;
    if (it != childNode->attributes.cend() && it->second.index() == 2) {
        const auto childName = std::get<std::string>(it->second);
        if (const auto p = m_links.find(childName); p != m_links.end())
            child = p->second;
    }

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
    std::pair<float, float> limits(0.0f, 0.0f);
    if (limitNode) {
        it = limitNode->attributes.find("lower");
        if (it != limitNode->attributes.cend() && it->second.index() == 1)
            limits.first = std::get<float>(it->second);
        else
            LOG_WARN << "No lower limit specified: " << name;
            
        it = limitNode->attributes.find("upper");
        if (it != limitNode->attributes.cend() && it->second.index() == 1)
            limits.second = std::get<float>(it->second);
        else
            LOG_WARN << "No upper limit specified: " << name;
    }
    else
        LOG_WARN << "No limits specified: " << name;

    // add joint
    LOG_INFO << "adding joint: " << name;
    m_joints.push_back(std::make_shared<JointData>(name, parent, child, t_child_parent, axis, limits));
    m_controlData.jointValues.push_back((limits.first+limits.second)/2.0f);

    return true;
}

void Robot::addEntity(const std::string& name, const std::shared_ptr<Entity>& entity) 
{ 
    assert(m_entities.find(name) == m_entities.end() && "Entity already exists");
    m_entities.emplace(name, entity); 
}

glm::mat4 Robot::forwardTransform()
{
    glm::mat4 t_child_world(1.0f);
    for (size_t i = 0; i < m_joints.size(); ++i) {
        auto& joint = m_joints[i];

        auto& parentLink = joint->parent;
        auto& childLink = joint->child;
        
        if (i == 0)
            parentLink->mesh->setTransformation(m_model);

        const auto t_parent_world = parentLink->mesh->getModel();
        t_child_world = joint->parentToChild * t_parent_world;
        t_child_world = glm::mat4(angleAxisF(m_controlData.jointValues[i], joint->rotationAxis)) * t_child_world;
          
        childLink->mesh->setTransformation(t_child_world);
        parentLink->frame->setTransformation(t_child_world);
        parentLink->frame->scale({400.0f, 400.0f, 400.0f});
        parentLink->frame->setVisible(m_controlData.drawFrames);
    }

    return t_child_world;
}