#include "pch.h"

#include "Camera.h"
#include "Scene.h"

#include "Window/Input.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"

#include "ImGui/ImGuiLayer.h"

#include "Util/geometry.h"

Camera::Camera()
    : m_transform(1.0f) , m_projection(1.0f), m_view(1.0f)
{
    
}

Camera::~Camera() = default;

void Camera::setProjection(const float hFov, const float zNear, const float zFar, const uint16_t width, const uint16_t height)
{
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    m_projection = glm::perspective(hFov, aspect, zNear, zFar);

    m_hFov = hFov;
    m_zNear = zNear;
    m_zFar = zFar;
}

void Camera::reset()
{
    m_transform = glm::mat4(1.0f);
    m_view = glm::mat4(1.0f);
}

void Camera::setTranslation(const glm::vec3& p_world)
{
    setMat4Translation(m_transform, p_world);
    posToView();
}

void Camera::setRotation(const float angle, const glm::vec3& v_axis_world)
{
    const glm::mat3 r_world = angleAxisF(angle, v_axis_world);
    setMat4Rotation(m_transform, r_world);
    posToView();
}

void Camera::setTransformation(const glm::mat4& t_cam_world)
{
    m_transform = t_cam_world;
    posToView();
}

void Camera::translateWorld(const glm::vec3& v_world)
{
    for (size_t i = 0; i < 3; ++i)
        m_transform[i][3] += v_world[i];
    posToView();
}

void Camera:: translate(const glm::vec3& v_cam)
{
    const auto v_world = getMat4AxisX(m_transform)*v_cam.x + getMat4AxisY(m_transform)*v_cam.y + getMat4AxisZ(m_transform)*v_cam.z;
    translateWorld(v_world);
}

void Camera::rotate(const float angle, const glm::vec3& v_axis_cam)
{
    const auto r_cam = angleAxisF(angle, v_axis_cam);
    m_transform = glm::mat4(r_cam) * m_transform;
    posToView();
}

void Camera::rotate(const glm::mat3& r_cam)
{
    m_transform = glm::mat4(r_cam) * m_transform;
    posToView();
}

void Camera::transform(const glm::mat4& t_cam)
{
    m_transform = t_cam * m_transform;
    posToView();
}

std::tuple<glm::vec3, glm::vec3> Camera::screenToCam(const glm::vec2& p_mouse_screen) const
{
    const float tanV = 1.0f / m_projection[1][1];  // Vertical tangent
    const float aspect = m_projection[1][1] / m_projection[0][0];  // Aspect ratio
    const float tanH = tanV * aspect;  // Horizontal tangent

    const float rightNear = m_zNear*tanH;
    const float topNear = m_zNear*tanV;
    const float leftNear = -rightNear;
    const float bottomNear = -topNear;

    const float rightFar = m_zFar*tanH;
    const float topFar = m_zFar*tanV;
    const float leftFar = -rightFar;
    const float bottomFar = -topFar;

    auto [width, height] = ImGuiLayer::getViewportSize();
    if (width <= 0 || height <= 0) {
        width = Window::getWidth();
        height = Window::getHeight();
    }

    glm::vec3 p_near_cam(
        -map(p_mouse_screen.x, 0, width, leftNear, rightNear),
        map(p_mouse_screen.y, 0, height, topNear, bottomNear),
        m_zNear
    );

    glm::vec3 p_far_cam(
        -map(p_mouse_screen.x, 0, width, leftFar, rightFar),
        map(p_mouse_screen.y, 0, height, topFar, bottomFar),
        m_zFar
    );

    return {p_near_cam, p_far_cam};
}

std::tuple<glm::vec3, glm::vec3> Camera::screenToWorld(const glm::vec2& p_mouse_screen) const
{
    const auto[p_near_cam, p_far_cam] = screenToCam(p_mouse_screen);

    const auto& t_cam_world = m_transform;
    const auto p_cam_world = getMat4Translation(t_cam_world);
    const auto v_camZ_world = getMat4AxisZ(t_cam_world);
    const auto v_camY_world = getMat4AxisY(t_cam_world);
    const auto v_camX_world = getMat4AxisX(t_cam_world);

    auto p_near_world = p_cam_world;
    p_near_world += v_camZ_world*p_near_cam.z;
    p_near_world += v_camY_world*p_near_cam.y;
    p_near_world += v_camX_world*p_near_cam.x;

    auto p_far_world = p_cam_world;
    p_far_world += v_camZ_world*p_far_cam.z;
    p_far_world += v_camY_world*p_far_cam.y;
    p_far_world += v_camX_world*p_far_cam.x;

    return {p_near_world, p_far_world};
}

std::tuple<glm::vec3, glm::vec3> Camera::ray(const glm::vec2& p_mouse_screen) const
{
    const auto[p_near_world, p_far_world] = screenToWorld(p_mouse_screen);

    const glm::vec3 v_ray_world = glm::normalize(p_far_world - p_near_world);
    const glm::vec3 p_ray_world = p_near_world;

    return {v_ray_world, p_ray_world};
}

void Camera::posToView()
{
    const auto p_cam_world = getMat4Translation(m_transform);
    const auto v_camZ_world = getMat4AxisZ(m_transform);
    const auto v_camY_world = getMat4AxisY(m_transform);
    const auto p_target_world = p_cam_world + v_camZ_world;
    m_view = glm::lookAt(p_cam_world, p_target_world, v_camY_world);
}

// ------------------------------------------------

Camera CameraController::s_camera;
float CameraController::s_hFov;
float CameraController::s_zFar;
float CameraController::s_zNear;
glm::mat4 CameraController::s_initialTransformation;

bool CameraController::s_draggingTrans;
bool CameraController::s_draggingRot;
glm::vec2 CameraController::s_screenPosPrev;
glm::mat4 CameraController::s_camPosPrev;
glm::vec3 CameraController::s_dragPos;

void CameraController::init(const float hFov, const float zNear, const float zFar, const glm::mat4& t_camInit_world)
{
    s_hFov = hFov;
    s_zNear = zNear;
    s_zFar = zFar;
    s_initialTransformation = t_camInit_world;

    s_draggingTrans = false;
    s_draggingRot = false;

    s_camera.setTransformation(t_camInit_world);
    updateProjection();
}

void CameraController::update(const Timestep dt)
{
    if (ImGuiLayer::isViewportFocused()) {
        if (Input::isKeyPressed(GLFW_KEY_A))
            s_camera.rotate(-200 * dt, {0.0f, 1.0f, 0.0f});

        else if(Input::isKeyPressed(GLFW_KEY_D))
            s_camera.rotate(200 * dt, {0.0f, 1.0f, 0.0f});

        if (Input::isKeyPressed(GLFW_KEY_W))
            s_camera.rotate(200 * dt, {1.0f, 0.0f, 0.0f});

        else if (Input::isKeyPressed(GLFW_KEY_S))
            s_camera.rotate(-200 * dt, {1.0f, 0.0f, 0.0f});
    }
}

void CameraController::onResize()
{
    updateProjection();
}

void CameraController::stopInteraction()
{
    stopDraggingTrans();
    stopDraggingRot();
}

void CameraController::startDraggingTrans(const glm::vec2& p_mouse_screen)
{
    if (s_draggingRot)
        return;

    s_draggingTrans = true;
    startDragging(p_mouse_screen);
}

void CameraController::stopDraggingTrans()
{
    s_dragPos = glm::vec3(0.0f);
    s_draggingTrans = false;
}

void CameraController::startDraggingRot(const glm::vec2& p_mouse_screen)
{
    if (s_draggingTrans)
        return;

    s_draggingRot = true;
    startDragging(p_mouse_screen);
}

void CameraController::startDragging(const glm::vec2& p_mouse_screen)
{
    const auto& t_cam_world = s_camera.getTransformation();
    const auto p_cam_world = getMat4Translation(t_cam_world);
    const auto ray_world = s_camera.ray(p_mouse_screen);

    float minDist = std::numeric_limits<float>::max();
    bool hit = false;
    glm::vec3 p_hitTmp_world, p_hit_world;
    
    auto view = Scene::s_registry.view<TriangulationComponent, TransformComponent, PropertiesComponent>();
    for (const auto [e, triangulation, transform, properties] : view.each()) {
        if (!properties.visible || !properties.clickable)
            continue;

        Entity entity{e};
        if (entity.hasComponent<BoundingBoxComponent>()) {
            const auto& boundingBox = entity.getComponent<BoundingBoxComponent>();
            if (!boundingBox.rayIntersects(ray_world))
                continue;
        }

        float dist;
        if (triangulation.rayIntersection(ray_world, p_hitTmp_world, dist)) {
            LOG_TRACE << entity.getTag();
            if (!hit || dist < minDist) {
                hit = true;
                minDist = dist;
                p_hit_world = p_hitTmp_world;
            }
        }
    }

    s_screenPosPrev = p_mouse_screen;
    s_camPosPrev = t_cam_world;
    s_dragPos = hit && !glm::any(glm::isnan(p_hit_world)) ? p_hit_world : glm::vec3(0.0f);
}

void CameraController::stopDraggingRot()
{
    s_dragPos = glm::vec3(0.0f);
    s_draggingRot = false;
}

void CameraController::drag(const glm::vec2& p_mouse_screen)
{
    auto t_cam_world = s_camera.getTransformation();

    if (glm::any(glm::isnan(t_cam_world[0])) || glm::any(glm::isnan(t_cam_world[1])) || glm::any(glm::isnan(t_cam_world[2])) || glm::any(glm::isnan(t_cam_world[3]))) {
        stopInteraction();
        s_camera.setTransformation(s_initialTransformation);
        return;
    }

    if (s_draggingTrans) {    
        const glm::vec2 v = s_dragFactor*glm::length(s_dragPos-getMat4Translation(t_cam_world))*(p_mouse_screen - s_screenPosPrev);
        s_camera.translate(glm::vec3(v.x, v.y, 0.0));
        s_camPosPrev = s_camera.getTransformation();
        s_screenPosPrev = p_mouse_screen;
    }

    else if (s_draggingRot) {       
        const auto angle = -deg2rad(s_rotFactor*glm::length(p_mouse_screen - s_screenPosPrev));

        const auto p_drag_world = s_dragPos;
        const auto v_camZ_world = getMat4AxisZ(t_cam_world);

        glm::vec3 p_prev_world, p_curr_world;
        const auto [v_rayPrev_world, p_rayPrev_world] = s_camera.ray(s_screenPosPrev);       
        if (!intersectionLinePlane(getMat4AxisZ(s_camPosPrev), p_drag_world, v_rayPrev_world, p_rayPrev_world, p_prev_world))
            return;
        const auto [v_rayCurr_world, p_rayCurr_world] = s_camera.ray(p_mouse_screen);
        if (!intersectionLinePlane(v_camZ_world, p_drag_world, v_rayCurr_world, p_rayCurr_world, p_curr_world))
            return;

        const auto v_drag_world = glm::normalize(p_curr_world - p_prev_world);
        const auto v_axis_world = glm::cross(v_drag_world, v_camZ_world);
        const auto t_world_cam = glm::inverseTranspose(t_cam_world);
        const auto v_axis_cam = t_world_cam * glm::vec4(v_axis_world, 0.0f);
        const auto r_drag_cam = angleAxisF(angle, v_axis_cam);

        rotateAroundPoint(p_drag_world, r_drag_cam, t_cam_world);
        s_camera.setTransformation(t_cam_world);

        s_camPosPrev = t_cam_world;
        s_screenPosPrev = p_mouse_screen;
    }
}

void CameraController::zoom(const float factor)
{
    const auto viewportPos = ImGuiLayer::screenToViewport(Input::GetMousePosition());
    const auto[v_ray_world, p_ray_world] = s_camera.ray(viewportPos);
    s_camera.translateWorld(factor*s_scrollFactor*v_ray_world);
}

void CameraController::updateProjection()
{
    assert(Window::isInitialized() && "Window not initialized");

    auto [width, height] = ImGuiLayer::getViewportSize();
    if (width <= 0 || height <= 0) {
        width = Window::getWidth();
        height = Window::getHeight();
    }

    glViewport(0, 0, width, height);
    s_camera.setProjection(s_hFov, s_zNear, s_zFar, width, height);
}
