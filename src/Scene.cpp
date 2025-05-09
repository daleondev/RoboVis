#include "pch.h"

#include "Scene.h"

#include "Window/Input.h"

#include "Renderer/Renderer.h"

#include "ImGui/ImGuiLayer.h"

#include "Entities/Frame.h"
#include "Entities/Mesh.h"
#include "Entities/Plane.h"
#include "Entities/Sphere.h"
#include "Entities/Robot.h"

#include "Util/geometry.h"

std::shared_ptr<FrameBuffer> Scene::s_frameBuffer;
std::unordered_map<std::string, std::shared_ptr<Entity>> Scene::s_entities;

void Scene::init()
{
    auto [width, height] = ImGuiLayer::getViewportSize();
    if (width <= 0 || height <= 0) {
        width = Window::getWidth();
        height = Window::getHeight();
    }
    s_frameBuffer = std::make_shared<FrameBuffer>(width, height);

    const auto r_cam_world = angleAxisF(M_PIf32/2 + M_PIf32/8, glm::vec3(1.0f, 0.0f, 0.0f)) * angleAxisF(-M_PIf32/4, glm::vec3(0.0f, 0.0f, 1.0f));
    const auto p_cam_world = glm::vec3(2000.0f, 2000.0f, 2000.0f);

    glm::mat4 t_cam_world(1.0f);
    setMat4Rotation(t_cam_world, r_cam_world);
    setMat4Translation(t_cam_world, p_cam_world);

    // auto origin = Scene::createFrame("Origin");
    // origin->scale({1000.0f, 1000.0f, 1000.0f});

    auto dragPoint = Scene::createSphere("DragPoint");
    dragPoint->scale({60.0f, 60.0f, 60.0f});
    dragPoint->setVisible(false);

    constexpr glm::vec4 light(0.9f, 0.9f, 0.9f, 1.0f);
    constexpr glm::vec4 dark(0.8f, 0.8f, 0.8f, 1.0f);

    const uint32_t texWidth = 12;
    const uint32_t texHeight = 12;
    std::vector<uint32_t> texData(texWidth*texHeight);
    for (uint32_t i = 0; i < texWidth; ++i) {
        for (uint32_t j = 0; j < texHeight; ++j) {
            if (i%2==0)
                texData[i*texWidth + j] = vecToRGBA(j%2 ? dark : light);
            else
                texData[i*texWidth + j] = vecToRGBA(j%2 ? light : dark);
        }
    }
    auto texture = TextureLibrary::create("Checkerboard", texData, texWidth, texHeight);
    createPlane("Plane", texture, glm::scale(glm::mat4(1.0f), {12000.0f, 12000.0f, 12000.0f}));

    CameraController::init(70.0f, 300.0f, 30000.0f, t_cam_world);
    Renderer::init();
}

std::shared_ptr<Robot> Scene::createRobot(const std::string& name, const std::filesystem::path& sourceDir, const glm::mat4& initialTransformation)
{
    std::shared_ptr<Robot> robot = std::make_shared<Robot>();
    if (!robot->setup(sourceDir))
        return nullptr;

    robot->setTransformation(initialTransformation);
    addEntity(name, robot);
    return robot;
}

std::shared_ptr<Mesh> Scene::createMesh(const std::string& name, const aiScene* source, const glm::mat4& t_mesh_world, const glm::mat4& initialTransformation)
{
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(source, t_mesh_world);
    mesh->setTransformation(initialTransformation);
    addEntity(name, mesh);
    return mesh;
}

std::shared_ptr<Frame> Scene::createFrame(const std::string& name, const glm::mat4& initialTransformation)
{
    std::shared_ptr<Frame> frame = std::make_shared<Frame>();
    frame->setTransformation(initialTransformation);
    addEntity(name, frame);
    return frame;
}

std::shared_ptr<Plane> Scene::createPlane(const std::string& name, const std::shared_ptr<Texture2D>& texture, const glm::mat4& initialTransformation)
{
    std::shared_ptr<Plane> plane = std::make_shared<Plane>(texture);
    plane->setTransformation(initialTransformation);
    addEntity(name, plane);
    return plane;
}

std::shared_ptr<Plane> Scene::createPlane(const std::string& name, const glm::vec4& color, const glm::mat4& initialTransformation)
{
    std::shared_ptr<Plane> plane = std::make_shared<Plane>(color);
    plane->setTransformation(initialTransformation);
    addEntity(name, plane);
    return plane;
}

std::shared_ptr<Sphere> Scene::createSphere(const std::string& name, const glm::vec4& color, const glm::mat4& initialTransformation)
{
    std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(color);
    sphere->setTransformation(initialTransformation);
    addEntity(name, sphere);
    return sphere;
}

void Scene::addEntity(const std::string& name, const std::shared_ptr<Entity>& entity) 
{ 
    assert(s_entities.find(name) == s_entities.end() && "Entity already exists");
    s_entities.emplace(name, entity); 
}

std::shared_ptr<Entity> Scene::getEntity(const std::string& name)  
{ 
    assert(s_entities.find(name) != s_entities.end() && "Entity doesnt exist");
    return s_entities[name];
}

void Scene::deleteEntity(const std::string& name) 
{ 
    auto it = s_entities.find(name);
    assert(it != s_entities.end() && "Entity doesnt exist");
    s_entities.erase(it);
}

void Scene::render(const Timestep dt)
{   
    s_frameBuffer->bind();
    Renderer::clear({218.0f/256, 237.0f/256, 245.0f/256, 1.0f}); 
    CameraController::update(dt);

    for (const auto&[name, entity] : s_entities) {
        if (ImGuiLayer::isViewportFocused()) {
            if (Input::isKeyPressed(GLFW_KEY_LEFT))
                entity->rotate(-500*dt, {0.0f, 1.0f, 0.0f});

            else if(Input::isKeyPressed(GLFW_KEY_RIGHT))
                entity->rotate(500*dt, {0.0f, 1.0f, 0.0f});

            if (Input::isKeyPressed(GLFW_KEY_UP))
                entity->rotate(-500*dt, {1.0f, 0.0f, 0.0f});

            else if(Input::isKeyPressed(GLFW_KEY_DOWN))
                entity->rotate(500*dt, {1.0f, 0.0f, 0.0f});
        }

        if (auto robot = dynamic_cast<Robot*>(entity.get()); robot != nullptr)
            robot->update(dt);

        entity->draw(CameraController::getCamera());
    }   
    s_frameBuffer->release();
}

bool Scene::onMouseLeave(MouseLeaveEvent& e)
{   
    CameraController::stopInteraction();

    return true;
}

bool Scene::onMouseMoved(MouseMovedEvent& e)
{   
    const auto viewportPos = ImGuiLayer::screenToViewport(e.getPosition());
    if (CameraController::isDragging()) {
        const auto pos = e.getPosition();
        CameraController::drag(viewportPos);
    }

    return true;
}

bool Scene::onMouseButtonPressed(MouseButtonPressedEvent& e)
{
    const auto viewportPos = ImGuiLayer::screenToViewport(Input::GetMousePosition());
    switch(e.getMouseButton()) {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            CameraController::startDraggingTrans(viewportPos);
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            CameraController::startDraggingRot(viewportPos);
            break;
        }
    }

    if (CameraController::isDragging() && entityExists("DragPoint")) {
        auto marker = getEntity("DragPoint");
        marker->setTranslation(CameraController::getDraggingPosition());
        marker->setVisible(true);
    }

    return true;
}

bool Scene::onMouseButtonReleased(MouseButtonReleasedEvent& e)
{
    switch(e.getMouseButton()) {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            CameraController::stopDraggingTrans();
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            CameraController::stopDraggingRot();
            if (entityExists("DragPoint"))
                getEntity("DragPoint")->setVisible(false);
            break;
        }
    }

    return true;
}

bool Scene::onMouseScrolled(MouseScrolledEvent& e)
{
    CameraController::zoom(e.getYOffset());

    return true;
}

bool Scene::onMouseDropped(MouseDroppedEvent& e)
{
    LOG_TRACE << e.toString();
    
    if (e.getNumPaths() == 1) {
        std::filesystem::path path = e.getPath(0);
        // folder -> robot
        if (ImGuiLayer::isViewportHovered() && std::filesystem::is_directory(path)) {
            createRobot("test", path);
        }
        // assimp extension -> mesh
        else if (ImGuiLayer::isViewportHovered() && !std::filesystem::is_directory(path) && aiIsExtensionSupported(path.extension().c_str()) == AI_TRUE) {

        }
        // trajectory file -> drag on specific robot control -> load trajectory for robot
        else if (ImGuiLayer::isViewportHovered() && !std::filesystem::is_directory(path) && path.extension().string() == ".txt") {
            auto robot = getEntity("robot");
            dynamic_cast<Robot*>(robot.get())->loadTrajectory(path);
        }
    }

    return true;
}

bool Scene::onWindowResized(WindowResizeEvent& e)
{
    CameraController::onResize();

    return true;
}