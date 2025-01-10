#include "pch.h"

#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Camera.h"

#include "Window/Input.h"

#include "Renderer/Renderer.h"

#include "ImGui/ImGuiLayer.h"

#include "Util/geometry.h"
#include "Util/MeshLoader.h"

std::shared_ptr<FrameBuffer> Scene::s_frameBuffer;
entt::registry Scene::s_registry;
std::unordered_map<UUID, Entity> Scene::s_entities;

struct EntitiyIds
{
    inline static UUID basePlateId = UUID_NULL;
    inline static UUID originId = UUID_NULL;
    inline static UUID dragMarkerId = UUID_NULL;
};

template<typename... Components>
static void setComponentsConstructCallback(entt::registry& registry, auto callback) {
    (..., registry.on_construct<Components>().template connect<[callback](entt::registry& registry, entt::entity handle) { 
        callback(registry, handle, registry.get<Components>(handle)); 
    }>());
}

void Scene::init()
{
    //------------------------------------------------------
    //                      Viewport
    //------------------------------------------------------

    auto [width, height] = ImGuiLayer::getViewportSize();
    if (width <= 0 || height <= 0) {
        width = Window::getWidth();
        height = Window::getHeight();
    }
    s_frameBuffer = std::make_shared<FrameBuffer>(width, height);

    //------------------------------------------------------
    //                      Camera
    //------------------------------------------------------

    const auto r_cam_world = angleAxisF(M_PIf32/2 + M_PIf32/8, glm::vec3(1.0f, 0.0f, 0.0f)) * angleAxisF(-M_PIf32/4, glm::vec3(0.0f, 0.0f, 1.0f));
    const auto p_cam_world = glm::vec3(2000.0f, 2000.0f, 2000.0f);

    glm::mat4 t_cam_world(1.0f);
    setMat4Rotation(t_cam_world, r_cam_world);
    setMat4Translation(t_cam_world, p_cam_world);

    //------------------------------------------------------
    //                      Registry
    //------------------------------------------------------

    setComponentsConstructCallback<PlaneRendererComponent, FrameRendererComponent, SphereRendererComponent>(s_registry, [](entt::registry& registry, Entity entity, auto component) -> void {
        entity.addComponent<VisibilityComponent>();
        auto& transform = entity.addComponent<TransformComponent>();   
        auto& triangulation = entity.addComponent<TriangulationComponent>();

        triangulation.data = component.createTriangulation();
        triangulation.update(transform.get());
    });

    s_registry.on_construct<MeshRendererComponent>().connect<[](entt::registry& registry, Entity entity) -> void {       
        entity.addComponent<VisibilityComponent>();
        auto& transform = entity.addComponent<TransformComponent>();
        auto& triangulation = entity.addComponent<TriangulationComponent>();       
        auto& boundingBox = entity.addComponent<BoundingBoxComponent>();

        triangulation.update(transform.get());
        boundingBox.update(triangulation.limits);
    }>();

    //------------------------------------------------------
    //                      Base Plate Plane
    //------------------------------------------------------
 
    {
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
        const auto texture = TextureLibrary::create("Checkerboard", texData, texWidth, texHeight);

        auto basePlate = createEntity("BasePlate");
        basePlate.addComponent<PlaneRendererComponent>(texture);
        EntitiyIds::basePlateId = basePlate.getComponent<IdComponent>().id;
        auto& trans = basePlate.getComponent<TransformComponent>();
        trans.scale = {12000.0f, 12000.0f, 12000.0f};
    }

    //------------------------------------------------------
    //                      Origin Frame
    //------------------------------------------------------

    {
        auto origin = createEntity("Origin");
        origin.addComponent<FrameRendererComponent>();
        EntitiyIds::originId = origin.getComponent<IdComponent>().id;
        auto& trans = origin.getComponent<TransformComponent>();
        trans.scale = {1000.0f, 1000.0f, 1000.0f};
    }

    //------------------------------------------------------
    //                      Drag Marker Sphere
    //------------------------------------------------------

    {
        auto dragMarker = createEntity("DragMarker");
        dragMarker.addComponent<SphereRendererComponent>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        EntitiyIds::dragMarkerId = dragMarker.getComponent<IdComponent>().id;
        auto& trans = dragMarker.getComponent<TransformComponent>();
        trans.scale = {60.0f, 60.0f, 60.0f};
    }

    //------------------------------------------------------
    //                      Init
    //------------------------------------------------------

    CameraController::init(70.0f, 300.0f, 30000.0f, t_cam_world);
    Renderer::init();
}

void Scene::update(const Timestep dt)
{   
    s_frameBuffer->bind();

    Renderer::clear({218.0f/256, 237.0f/256, 245.0f/256, 1.0f}); 
    CameraController::update(dt);   

    // draw planes
    {
        auto group = s_registry.group<PlaneRendererComponent>(entt::get_t<TransformComponent, VisibilityComponent, TriangulationComponent>());
        for (auto [entity, plane, transform, visibility, triangulation] : group.each()) {
            if (!visibility.visible)
                continue;

            triangulation.update(transform.get());

            if (plane.hasTexture())
                Renderer::drawPlane(transform.get(), std::get<std::shared_ptr<Texture2D>>(plane.material));
            else
                Renderer::drawPlane(transform.get(), std::get<glm::vec4>(plane.material));
        }
    }

    // draw frames
    {
        auto group = s_registry.group<FrameRendererComponent>(entt::get_t<TransformComponent, VisibilityComponent, TriangulationComponent>());
        for (auto [entity, frame, transform, visibility, triangulation] : group.each()) {
            if (!visibility.visible)
                continue;

            triangulation.update(transform.get());
            Renderer::drawFrame(transform.get());
        }
    }

    // draw spheres
    {
        auto group = s_registry.group<SphereRendererComponent>(entt::get_t<TransformComponent, VisibilityComponent, TriangulationComponent>());
        for (auto [entity, sphere, transform, visibility, triangulation] : group.each()) {
            if (!visibility.visible)
                continue;

            triangulation.update(transform.get());
            Renderer::drawSphere(transform.get(), sphere.color);
        }
    }

    // draw meshes
    {
        auto group = s_registry.group<MeshRendererComponent>(entt::get_t<TransformComponent, VisibilityComponent, TriangulationComponent, BoundingBoxComponent>());
        for (auto [entity, mesh, transform, visibility, triangulation, boundingBox] : group.each()) {
            if (!visibility.visible)
                continue;

            triangulation.update(transform.get());
            boundingBox.update(triangulation.limits);

            Renderer::drawMesh(transform.get(), mesh.id);
            if (boundingBox.visible)
                Renderer::drawBox(transform.get(), mesh.id, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        }
    }
    
    s_frameBuffer->release();
}

Entity Scene::createEntity(const std::string& tag)
{
    Entity entity(s_registry.create());

    auto& idComponent = entity.addComponent<IdComponent>();
    entity.addComponent<TagComponent>(tag.empty() ? "Entity" : tag);
    
    s_entities.emplace(idComponent.id, entity);
    return entity;
}

void Scene::destroyEntity(const Entity entity)
{
    s_entities.erase(entity.getId());
    s_registry.destroy(entity);
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

    if (CameraController::isDragging() && EntitiyIds::dragMarkerId != UUID_NULL) {
        const auto entity = s_entities.at(EntitiyIds::dragMarkerId);
        auto [transform, visibility] = s_registry.get<TransformComponent, VisibilityComponent>(entity);
        transform.translation = CameraController::getDraggingPosition();
        visibility.visible = true;
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
            if (EntitiyIds::dragMarkerId != UUID_NULL) {
                const auto entity = s_entities.at(EntitiyIds::dragMarkerId);
                s_registry.get<VisibilityComponent>(entity).visible = false;
            }
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
            // createRobot("test", path);
        }
        // assimp extension -> mesh
        else if (ImGuiLayer::isViewportHovered() && !std::filesystem::is_directory(path) && aiIsExtensionSupported(path.extension().c_str()) == AI_TRUE) {
            auto mesh = MeshLibrary::load(path);
            Renderer::addMeshData(mesh->id, mesh->data);

            auto meshEntity = createEntity("Mesh");
            meshEntity.addComponent<MeshRendererComponent>(mesh->id);
            auto& triangulation = meshEntity.getComponent<TriangulationComponent>();
            for (auto& vertex : mesh->data.vertices)
                triangulation.data->vertices.emplace_back(vertex.position);
            triangulation.data->indices = mesh->data.indices;

            auto& boundingBox = meshEntity.getComponent<BoundingBoxComponent>();
            triangulation.update(glm::mat4(1.0f));
            boundingBox.update(triangulation.limits);

            BoxData boxData;
            boxData.vertices = boundingBox.data->vertices;
            Renderer::addBoxData(mesh->id, boxData);

            LOG_TRACE << "added mesh " << path;
        }
        // trajectory file -> drag on specific robot control -> load trajectory for robot
        else if (ImGuiLayer::isViewportHovered() && !std::filesystem::is_directory(path) && path.extension().string() == ".txt") {
            // auto robot = getEntity("robot");
            // dynamic_cast<Robot*>(robot.get())->loadTrajectory(path);
        }
    }

    return true;
}

bool Scene::onWindowResized(WindowResizeEvent& e)
{
    CameraController::onResize();

    return true;
}