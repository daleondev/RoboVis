#include "pch.h"

#include "Camera.h"
#include "Scene.h"

#include "Window/Window.h"
#include "Window/Input.h"

#include "Entities/Mesh.h"

#include "geometry.h"

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

void Camera::setTranslation(const glm::vec3& trans)
{
    m_pos = setMat4Translation(m_pos, trans);
    posToView();
}

void Camera::setRotation(const float angle, const glm::vec3& axis)
{
    const glm::mat3 rot = anlgeAxisF(angle, axis);
    m_pos = setMat4Rotation(m_pos, rot);
    posToView();
}

void Camera::setTransformation(const glm::mat4& transformation)
{
    m_pos = transformation;
    posToView();
}

void Camera::translateWorld(const glm::vec3& translation)
{
    for (size_t i = 0; i < 3; ++i)
        m_pos[i][3] += translation[i];
    posToView();
}

void Camera:: translateLocal(const glm::vec3& translation)
{
    const auto t = getMat4AxisX(m_pos)*translation.x + getMat4AxisY(m_pos)*translation.y + getMat4AxisZ(m_pos)*translation.z;
    for (size_t i = 0; i < 3; ++i)
        m_pos[i][3] += t[i];
    posToView();
}

void Camera::rotate(const float angle, const glm::vec3& axis)
{
    auto R = anlgeAxisF(angle, axis);
    m_pos = glm::mat4(R) * m_pos;
    posToView();
}

void Camera::rotate(const glm::mat3& R)
{
    m_pos = glm::mat4(R) * m_pos;
    posToView();
}

void Camera::transform(const glm::mat4& transformation)
{
    m_pos = transformation * m_pos;
    posToView();
}

void Camera::posToView()
{
    const auto trans = MAT_COL3(m_pos, 3);
    const auto dirZ = MAT_COL3(m_pos, 2);
    const auto dirY = MAT_COL3(m_pos, 1);
    const auto target = trans + dirZ;
    m_view = glm::lookAt(trans, target, dirY);;
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
std::optional<glm::vec3> CameraController::s_dragPos;

void CameraController::init(const float hFov, const float zNear, const float zFar, const glm::mat4& initialTransformation)
{
    s_hFov = hFov;
    s_zNear = zNear;
    s_zFar = zFar;
    s_initialTransformation = initialTransformation;

    s_draggingTrans = false;
    s_draggingRot = false;

    s_camera.setTransformation(initialTransformation);
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

void CameraController::startDraggingTrans(const glm::vec2& screenPos)
{
    if (s_draggingRot)
        return;

    s_draggingTrans = true;
    s_screenPosPrev = screenPos;
    s_camPosPrev = s_camera.getView();
    s_dragPos = {};
}

void CameraController::stopDraggingTrans()
{
    s_draggingTrans = false;
}

void CameraController::startDraggingRot(const glm::vec2& screenPos)
{
    if (s_draggingTrans)
        return;

    const glm::mat4 camPos = s_camera.getPosition();
    const glm::vec3 camOrig = -MAT_COL3(camPos, 3);
    const auto&[l, l0] = cameraRay(screenPos, camPos);
    
    float minDist = std::numeric_limits<float>::max();
    std::optional<glm::vec3> hitPos;
    for (auto&[name, entity] : Scene::getEntities()) {
        if(Mesh* mesh = dynamic_cast<Mesh*>(entity.get()); mesh != nullptr) {
            const auto& meshData = mesh->getData();

            for (const auto& data : meshData) {
                for (const auto& indices : data.indices) {
                    std::array<glm::vec3, 3> v;
                    for (size_t i = 0; i < 3; ++i)
                        v[i] = data.vertices[indices[i]].pos;

                    const auto&[n, p0] = trianglePlane(v);
                    const auto p = intersectionLinePlane(n, p0, l, l0);
                    if (p) {
                        if(pointInTriangle(n, p0, v, *p)) {
                            const float dist = glm::length(*p - camOrig);
                            if (!hitPos || dist < minDist) {
                                hitPos = *p;
                                minDist = dist;
                            }
                        }
                    }
                }
            }
        }
    }

    s_draggingRot = true;
    s_screenPosPrev = screenPos;
    s_camPosPrev = s_camera.getPosition();
    s_dragPos = hitPos && !glm::any(glm::isnan(*hitPos)) ? *hitPos : glm::vec3(0.0f);

    // auto[cam, _1] = CameraController::screenToCam(s_screenPosPrev, s_camPosPrev);
    // auto[world, _2] = CameraController::screenToWorld(s_screenPosPrev, s_camPosPrev);    
    // // auto[l, l0] = CameraController::cameraRay(s_screenPosPrev, s_camPosPrev); 
    // printVec(cam);
    // printVec(world);
    
    
    // auto dist = glm::length(*s_dragPos - getMat4Translation(s_camPosPrev));
    // auto worldt = getMat4Translation(s_camPosPrev) + dist*l;
    // printVec(worldt);
    // std::cout << "-----------------\n";
}

void CameraController::stopDraggingRot()
{
    s_dragPos = {};
    s_draggingRot = false;
}

void CameraController::drag(const glm::vec2& screenPos)
{
    const auto camPos = s_camera.getPosition();

    if (glm::any(glm::isnan(camPos[0])) || glm::any(glm::isnan(camPos[1])) || glm::any(glm::isnan(camPos[2])) || glm::any(glm::isnan(camPos[3]))) {
        stopInteraction();
        s_camera.setTransformation(s_initialTransformation);
        return;
    }

    if (s_draggingTrans) {
        const glm::vec2 v = -s_dragFactor*(screenPos - s_screenPosPrev);
        s_camera.translateLocal(glm::vec3(v.x, v.y, 0.0));
        s_camPosPrev = s_camera.getPosition();
        s_screenPosPrev = screenPos;
    }

    else if (s_draggingRot) {       
        const auto angle = -deg2rad(s_rotFactor*glm::length(screenPos - s_screenPosPrev));

        const auto camOrig = getMat4Translation(camPos);
        const auto camAxisZ = getMat4AxisZ(camPos);

        const auto camToDrag = *s_dragPos - camOrig;
        const auto camToDragLocal = glm::inverseTranspose(camPos) * glm::vec4(camToDrag, 0.0f);
        const auto distCamToDrag = glm::length(camToDrag);

        auto screenPrev = s_screenPosPrev; screenPrev.y = Window::getHeight()-screenPrev.y;
        auto screenCurr = screenPos; screenCurr.y = Window::getHeight()-screenCurr.y;

        const auto[lPrev, l0Prev] = CameraController::cameraRay(screenPrev, s_camPosPrev);
        const auto prev = getMat4Translation(s_camPosPrev) + glm::length(*s_dragPos-getMat4Translation(s_camPosPrev))*lPrev;
        const auto[lCurr, l0PCurr] = CameraController::cameraRay(screenCurr, camPos);
        const auto curr = camOrig + distCamToDrag*lCurr;

        // auto[prev, _1] = CameraController::screenToWorld(screenPrev, s_camPosPrev);
        // auto[curr, _2] = CameraController::screenToWorld(screenCurr, camPos);

        const auto dragDir = glm::normalize(curr - prev);
        const auto axis = cross(dragDir, camAxisZ);
        const auto rot = anlgeAxisF(angle, axis);

        s_camera.translateLocal(camToDragLocal);
        s_camera.rotate(rot);
        s_camera.translateLocal(-camToDragLocal);

        s_camPosPrev = s_camera.getPosition();
        s_screenPosPrev = screenPos;
    }
}

void CameraController::zoom(const float factor)
{
    auto[l, l0] = cameraRay(Input::GetMousePosition(), s_camera.getPosition());
    s_camera.translateWorld(factor*s_scrollFactor*l);
}

void CameraController::updateProjection()
{
    assert(Window::isInitialized() && "Window not initialized");

    float aspect = static_cast<float>(Window::getWidth()) / static_cast<float>(Window::getHeight());
    s_camera.setProjection(glm::perspective(s_hFov, aspect, s_zNear, s_zFar));
}

std::tuple<glm::vec3, glm::vec3> CameraController::screenToWorld(const glm::vec2& screenPos, const glm::mat4& camPos)
{
    const auto[camNear, camFar] = screenToCam(screenPos, camPos);

    const glm::vec3 camOrig = getMat4Translation(camPos);
    const glm::vec3 camAxisZ = getMat4AxisZ(camPos);
    const glm::vec3 camAxisY = getMat4AxisY(camPos);
    const glm::vec3 camAxisX = getMat4AxisX(camPos);

    glm::vec3 near = camOrig;
    near += camAxisX*camNear.x;
    near += camAxisY*camNear.y;
    near += camAxisZ*camNear.z;

    glm::vec3 far = camOrig;
    far += camAxisX*camFar.x;
    far += camAxisY*camFar.y;
    far += camAxisZ*camFar.z;

    return {near, far};
}

std::tuple<glm::vec3, glm::vec3> CameraController::screenToCam(const glm::vec2& screenPos, const glm::mat4& camPos)
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

    glm::vec3 near;
    near.x = -map(screenPos.x, 0, Window::getWidth(), leftNear, rightNear);
    near.y = map(screenPos.y, 0, Window::getHeight(), topNear, bottomNear);
    near.z = s_zNear;

    glm::vec3 far;
    far.x = -map(screenPos.x, 0, Window::getWidth(), leftFar, rightFar);
    far.y = map(screenPos.y, 0, Window::getHeight(), topFar, bottomFar);
    far.z = s_zFar;

    return {near, far};
}

std::tuple<glm::vec3, glm::vec3> CameraController::cameraRay(const glm::vec2& screenPos, const glm::mat4& camPos)
{
    const auto[near, far] = screenToWorld(screenPos, camPos);

    const glm::vec3 l0 = near;
    const glm::vec3 l = glm::normalize(far - near);

    return {l, l0};
}
