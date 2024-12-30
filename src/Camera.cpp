#include "Camera.h"

#include "Window.h"
#include "Scene.h"
#include "Input.h"

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

void Camera::setTranslation(const glm::vec3& translation)
{
    for (size_t i = 0; i < 3; ++i)
        m_pos[i][3] = translation[i];
    posToView();
}

void Camera::setRotation(const float angle, const glm::vec3& axis)
{
    const glm::mat3 R = anlgeAxisF(angle, axis);
    for (size_t i = 0; i < 3; ++i)
        for (size_t j = 0; j < 3; ++j)
            m_pos[i][j] = R[i][j];
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
    const auto dirX = MAT_COL3(m_pos, 0);
    const auto dirY = MAT_COL3(m_pos, 1);
    const auto dirZ = MAT_COL3(m_pos, 2);
    const auto t = dirX*translation.x + dirY*translation.y + dirZ*translation.z;
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
        stopDraggingTrans();
        stopDraggingRot();
        s_camera.reset();
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

        auto camOrig = MAT_COL3(camPos, 3);
        auto camDirZ = MAT_COL3(camPos, 2);
        auto camDirY = MAT_COL3(camPos, 1);
        auto camDirX = MAT_COL3(camPos, 0);
       
        auto screenPrev = s_screenPosPrev; screenPrev.y = Window::getHeight()-screenPrev.y;
        auto screenCurr = screenPos; screenCurr.y = Window::getHeight()-screenCurr.y;
        auto[_1, prev] = CameraController::cameraRay(screenPrev, s_camPosPrev);
        auto[_2, curr] = CameraController::cameraRay(screenCurr, camPos);

        const auto dragDir = glm::normalize(curr - prev);
        const auto axis = cross(dragDir, camDirZ);
        const auto rot = anlgeAxisF(angle, axis);
        const auto camToDrag = *s_dragPos - camOrig;
        const auto camToDragLocal = glm::inverseTranspose(camPos) * glm::vec4(camToDrag, 0.0f);

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

std::tuple<glm::vec3, glm::vec3> CameraController::cameraRay(const glm::vec2& screenPos, const glm::mat4& camPos)
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
    near.x = map(screenPos.x, 0, Window::getWidth(), leftNear, rightNear);
    near.y = map(screenPos.y, 0, Window::getHeight(), topNear, bottomNear);
    near.z = s_zNear;

    glm::vec3 far;
    far.x = map(screenPos.x, 0, Window::getWidth(), leftFar, rightFar);
    far.y = map(screenPos.y, 0, Window::getHeight(), topFar, bottomFar);
    far.z = s_zFar;

    const glm::vec3 camOrig = MAT_COL3(camPos, 3);
    const glm::vec3 camDirZ = MAT_COL3(camPos, 2);
    const glm::vec3 camDirY = MAT_COL3(camPos, 1);
    const glm::vec3 camDirX = -MAT_COL3(camPos, 0);

    glm::vec3 camNear = camOrig;
    camNear += camDirX*near.x;
    camNear += camDirY*near.y;
    camNear += camDirZ*near.z;

    glm::vec3 camFar = camOrig;
    camFar += camDirX*far.x;
    camFar += camDirY*far.y;
    camFar += camDirZ*far.z;

    const glm::vec3 l0 = camNear;
    const glm::vec3 l = glm::normalize(camFar - camNear);

    return {l, l0};
}
