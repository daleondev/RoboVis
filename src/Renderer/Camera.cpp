#include "pch.h"

#include "Camera.h"
#include "Scene.h"

#include "Window/Window.h"
#include "Window/Input.h"

#include "Entities/Mesh.h"

#include "Util/geometry.h"

Camera::Camera()
    : m_pos(1.0f) , m_projection(1.0f), m_view(1.0f)
{
    
}

Camera::~Camera() = default;

void Camera::setProjection(const glm::mat4& projection)
{
    m_projection = projection;
}

void Camera::reset()
{
    m_pos = glm::mat4(1.0f);
    m_view = glm::mat4(1.0f);
}

void Camera::setTranslation(const glm::vec3& p_world)
{
    setMat4Translation(m_pos, p_world);
    posToView();
}

void Camera::setRotation(const float angle, const glm::vec3& v_axis_world)
{
    const glm::mat3 r_world = angleAxisF(angle, v_axis_world);
    setMat4Rotation(m_pos, r_world);
    posToView();
}

void Camera::setTransformation(const glm::mat4& t_cam_world)
{
    m_pos = t_cam_world;
    posToView();
}

void Camera::translateWorld(const glm::vec3& v_world)
{
    for (size_t i = 0; i < 3; ++i)
        m_pos[i][3] += v_world[i];
    posToView();
}

void Camera:: translate(const glm::vec3& v_cam)
{
    const auto v_world = getMat4AxisX(m_pos)*v_cam.x + getMat4AxisY(m_pos)*v_cam.y + getMat4AxisZ(m_pos)*v_cam.z;
    translateWorld(v_world);
}

void Camera::rotate(const float angle, const glm::vec3& v_axis_cam)
{
    const auto r_cam = angleAxisF(angle, v_axis_cam);
    m_pos = glm::mat4(r_cam) * m_pos;
    posToView();
}

void Camera::rotate(const glm::mat3& r_cam)
{
    m_pos = glm::mat4(r_cam) * m_pos;
    posToView();
}

void Camera::transform(const glm::mat4& t_cam)
{
    m_pos = t_cam * m_pos;
    posToView();
}

void Camera::posToView()
{
    const auto p_cam_world = getMat4Translation(m_pos);
    const auto v_camZ_world = getMat4AxisZ(m_pos);
    const auto v_camY_world = getMat4AxisY(m_pos);
    const auto p_target_world = p_cam_world + v_camZ_world;
    m_view = glm::lookAt(p_cam_world, p_target_world, v_camY_world);;
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

void CameraController::onUpdate(const Timestep dt)
{
    if (Input::isKeyPressed(GLFW_KEY_A))
        s_camera.rotate(-200 * dt, {0.0f, 1.0f, 0.0f});

    else if(Input::isKeyPressed(GLFW_KEY_D))
        s_camera.rotate(200 * dt, {0.0f, 1.0f, 0.0f});

    if (Input::isKeyPressed(GLFW_KEY_W))
        s_camera.rotate(200 * dt, {1.0f, 0.0f, 0.0f});

    else if (Input::isKeyPressed(GLFW_KEY_S))
        s_camera.rotate(-200 * dt, {1.0f, 0.0f, 0.0f});
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
    s_screenPosPrev = p_mouse_screen;
    s_camPosPrev = s_camera.getView();
    s_dragPos = {};
}

void CameraController::stopDraggingTrans()
{
    s_draggingTrans = false;
}

void CameraController::startDraggingRot(const glm::vec2& p_mouse_screen)
{
    if (s_draggingTrans)
        return;

    const glm::mat4 t_cam_world = s_camera.getPosition();
    const glm::vec3 p_cam_world = -MAT_COL3(t_cam_world, 3);
    const auto&[v_ray_world, p_ray_world] = cameraRay(p_mouse_screen, t_cam_world);
    
    float minDist = std::numeric_limits<float>::max();
    std::optional<glm::vec3> p_hit_world;
    for (auto&[name, entity] : Scene::getEntities()) {
        if(Mesh* mesh = dynamic_cast<Mesh*>(entity.get()); mesh != nullptr) {
            const auto& meshData = mesh->getData();

            for (const auto& data : meshData) {
                for (const auto& indices : data.indices) {
                    std::array<glm::vec3, 3> p_vertices_world;
                    for (size_t i = 0; i < 3; ++i)
                        p_vertices_world[i] = data.vertices[indices[i]].pos;

                    const auto[v_n_world, v_p0_world] = trianglePlane(p_vertices_world);
                    const auto p_hitTmp_world = intersectionLinePlane(v_n_world, v_p0_world, v_ray_world, p_ray_world);                    
                    if (p_hitTmp_world) {                       
                        if(pointInTriangle(v_n_world, v_p0_world, p_vertices_world, *p_hitTmp_world)) {
                            const float dist = glm::length(*p_hitTmp_world - p_cam_world);
                            if (!p_hit_world || dist < minDist) {
                                p_hit_world = *p_hitTmp_world;
                                minDist = dist;
                            }
                        }
                    }
                }
            }
        }
    }

    s_draggingRot = true;
    s_screenPosPrev = p_mouse_screen;
    s_camPosPrev = t_cam_world;
    s_dragPos = glm::vec3(0.0f); // s_dragPos = p_hit_world && !glm::any(glm::isnan(*p_hit_world)) ? *p_hit_world : glm::vec3(0.0f);
}

void CameraController::stopDraggingRot()
{
    s_dragPos = {};
    s_draggingRot = false;
}

void CameraController::drag(const glm::vec2& p_mouse_screen)
{
    auto t_cam_world = s_camera.getPosition();

    if (glm::any(glm::isnan(t_cam_world[0])) || glm::any(glm::isnan(t_cam_world[1])) || glm::any(glm::isnan(t_cam_world[2])) || glm::any(glm::isnan(t_cam_world[3]))) {
        stopInteraction();
        s_camera.setTransformation(s_initialTransformation);
        return;
    }

    if (s_draggingTrans) {
        const glm::vec2 v = s_dragFactor*(p_mouse_screen - s_screenPosPrev);
        s_camera.translate(glm::vec3(v.x, v.y, 0.0));
        s_camPosPrev = s_camera.getPosition();
        s_screenPosPrev = p_mouse_screen;
    }

    else if (s_draggingRot) {       
        const auto angle = -deg2rad(s_rotFactor*glm::length(p_mouse_screen - s_screenPosPrev));

        const auto p_drag_world = s_dragPos;
        const auto v_camZ_world = getMat4AxisZ(t_cam_world);

        const auto[v_rayPrev_world, p_rayPrev_world] = CameraController::cameraRay(s_screenPosPrev, s_camPosPrev);
        const auto p_prev_world = *intersectionLinePlane(getMat4AxisZ(s_camPosPrev), p_drag_world, v_rayPrev_world, p_rayPrev_world);
        const auto[v_rayCurr_world, p_rayCurr_world] = CameraController::cameraRay(p_mouse_screen, t_cam_world);
        const auto p_curr_world = *intersectionLinePlane(v_camZ_world, p_drag_world, v_rayCurr_world, p_rayCurr_world);

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
    const auto[v_ray_world, p_ray_world] = cameraRay(Input::GetMousePosition(), s_camera.getPosition());
    s_camera.translateWorld(factor*s_scrollFactor*v_ray_world);
}

void CameraController::updateProjection()
{
    assert(Window::isInitialized() && "Window not initialized");

    float aspect = static_cast<float>(Window::getWidth()) / static_cast<float>(Window::getHeight());
    s_camera.setProjection(glm::perspective(s_hFov, aspect, s_zNear, s_zFar));
}

std::tuple<glm::vec3, glm::vec3> CameraController::screenToWorld(const glm::vec2& p_mouse_screen, const glm::mat4& t_cam_world)
{
    const auto[p_near_cam, p_far_cam] = screenToCam(p_mouse_screen);

    const glm::vec3 p_cam_world = getMat4Translation(t_cam_world);
    const glm::vec3 v_camZ_world = getMat4AxisZ(t_cam_world);
    const glm::vec3 v_camY_world = getMat4AxisY(t_cam_world);
    const glm::vec3 v_camX_world = getMat4AxisX(t_cam_world);

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

std::tuple<glm::vec3, glm::vec3> CameraController::screenToCam(const glm::vec2& p_mouse_screen)
{
    const float aspect = static_cast<float>(Window::getWidth()) / static_cast<float>(Window::getHeight());
    const float tangent = tanf(deg2rad(s_hFov/2.0f));

    const float rightNear = s_zNear*tangent;
    const float topNear = rightNear/aspect;
    const float leftNear = -rightNear;
    const float bottomNear = -topNear;

    const float rightFar = s_zFar*tangent;
    const float topFar = rightFar/aspect;
    const float leftFar = -rightFar;
    const float bottomFar = -topFar;

    glm::vec3 p_near_cam;
    p_near_cam.x = -map(p_mouse_screen.x, 0, Window::getWidth(), leftNear, rightNear);
    p_near_cam.y = map(p_mouse_screen.y, 0, Window::getHeight(), topNear, bottomNear);
    p_near_cam.z = s_zNear;

    glm::vec3 p_far_cam;
    p_far_cam.x = -map(p_mouse_screen.x, 0, Window::getWidth(), leftFar, rightFar);
    p_far_cam.y = map(p_mouse_screen.y, 0, Window::getHeight(), topFar, bottomFar);
    p_far_cam.z = s_zFar;

    return {p_near_cam, p_far_cam};
}

std::tuple<glm::vec3, glm::vec3> CameraController::cameraRay(const glm::vec2& p_mouse_screen, const glm::mat4& t_cam_world)
{
    const auto[p_near_world, p_far_world] = screenToWorld(p_mouse_screen, t_cam_world);

    const glm::vec3 v_ray_world = glm::normalize(p_far_world - p_near_world);
    const glm::vec3 p_ray_world = p_near_world;

    return {v_ray_world, p_ray_world};
}
