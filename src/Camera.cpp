#include "Camera.h"

#include "Window.h"
#include "Input.h"

#include "geometry.h"

Camera::Camera()
    : m_projection{ glm::mat4(1.0f) }, m_view{ glm::mat4(1.0f) }
{
    
}

Camera::~Camera() = default;

void Camera::setProjection(const glm::mat4& projection)
{
    m_projection = projection;
}

void Camera::reset()
{
    m_view = glm::mat4(1.0f);
}

void Camera::setTranslation(const glm::vec3& translation)
{
    for (size_t i = 0; i < 3; ++i)
        m_view[3][i] = translation[i];
}

void Camera::setRotation(const float angle, const glm::vec3& axis)
{
    const glm::mat3 rot = glm::toMat3(glm::angleAxis(angle, axis));
    for (size_t i = 0; i < 3; ++i)
        for (size_t j = 0; j < 3; ++j)
            m_view[i][j] = rot[i][j];
}

void Camera::setTransformation(const glm::mat4& transformation)
{
    m_view = transformation;
}

void Camera::translate(const glm::vec3& translation)
{
    m_view = glm::translate(m_view, translation);
}

void Camera::rotate(const float angle, const glm::vec3& axis)
{
    m_view = glm::rotate(m_view, angle, axis);
}

void Camera::transform(const glm::mat4& transformation)
{
    m_view *= transformation;
}    

glm::mat4 Camera::getProjection() const
{
    return m_projection;
}

glm::mat4 Camera::getView() const
{
    return m_view;
}

// ------------------------------------------------

Camera CameraController::s_camera;
float CameraController::s_hFov;
float CameraController::s_zFar;
float CameraController::s_zNear;

bool CameraController::s_draggingTrans;
bool CameraController::s_draggingRot;
glm::vec2 CameraController::s_dragStart;
glm::mat4 CameraController::s_camStart;
std::optional<glm::vec3> CameraController::s_entityStart;

static CameraParameters calculateCameraIntrinsics(const float width, const float height, const float hfov)
{
    float x = width;
    float y = height;

    float cx = x / 2.0f;
    float cy = y / 2.0f;

    float ax = deg2rad(hfov);
    float ay = 2 * atan(y * tan(ax / 2) / x);

    float fx = cx / tan(ax / 2);
    float fy = cy / tan(ay / 2);

    CameraParameters params;
    params.width = width;
    params.height = height;
    params.hfov = hfov;
    // +--------+
    // |fx 0  cx|
    // |0  fy cy|
    // |0  0  1 |
    // +--------+
    params.intrinsics = {
        glm::vec3{fx  , 0.0f, cx  },
        glm::vec3{0.0f, fy  , cy  },
        glm::vec3{0.0f, 0.0f, 1.0f}
    };

    return params;
}

void CameraController::init(const float hFov, const float zNear, const float zFar, const glm::mat4& initialTransformation)
{
    s_hFov = hFov;
    s_zNear = zNear;
    s_zFar = zFar;

    s_draggingTrans = false;
    s_draggingRot = false;

    // glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 20.0f);  
    // glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
    // glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    // glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    // s_camera.setTransformation(glm::lookAt(cameraPos, cameraTarget, cameraUp));

    s_camera.setTransformation(initialTransformation);

    updateProjection();
}

void CameraController::onUpdate(const Timestep dt)
{
    if (Input::isKeyPressed(GLFW_KEY_A))
        s_camera.translate({-s_transFactor * dt, 0.0f, 0.0f});

    else if(Input::isKeyPressed(GLFW_KEY_D))
        s_camera.translate({s_transFactor * dt, 0.0f, 0.0f});

    if (Input::isKeyPressed(GLFW_KEY_W))
        s_camera.translate({0.0f, s_transFactor * dt, 0.0f});

    else if (Input::isKeyPressed(GLFW_KEY_S))
        s_camera.translate({0.0f, -s_transFactor * dt, 0.0f});
}

void CameraController::resize()
{
    updateProjection();
}

void CameraController::stopInteraction()
{
    stopDraggingTrans();
    stopDraggingRot();
}

void CameraController::startDraggingTrans(const glm::vec2& pos)
{
    if (s_draggingRot)
        return;

    s_draggingTrans = true;
    s_dragStart = pos;
    s_camStart = s_camera.getView();
    s_entityStart = {};
}

void CameraController::stopDraggingTrans()
{
    s_draggingTrans = false;
}

void CameraController::startDraggingRot(const glm::vec2& pos, const aiScene* scene)
{
    if (s_draggingTrans)
        return;

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
    near.x = map(pos.x, 0, Window::getWidth(), leftNear, rightNear);
    near.y = map(pos.y, 0, Window::getHeight(), topNear, bottomNear);
    near.z = s_zNear;

    glm::vec3 far;
    far.x = map(pos.x, 0, Window::getWidth(), leftFar, rightFar);
    far.y = map(pos.y, 0, Window::getHeight(), topFar, bottomFar);
    far.z = s_zFar;

    const auto view = s_camera.getView();
    const glm::vec3 camPos = -view[3];
    const glm::vec3 camZ = -view[2];
    const glm::vec3 camY = view[1];
    const glm::vec3 camX = view[0];

    glm::vec3 camNear = camPos;
    camNear += camX*near.x;
    camNear += camY*near.y;
    camNear += camZ*near.z;

    glm::vec3 camFar = camPos;
    camFar += camX*far.x;
    camFar += camY*far.y;
    camFar += camZ*far.z;

    const glm::vec3 l0 = camNear;
    const glm::vec3 l = camFar - camNear;

    // std::cout << l0[0] << " "  << l0[1] << " "  << l0[2] << "\n";
    // std::cout << l[0] << " "  << l[1] << " "  << l[2] << "\n";

    float minDist = std::numeric_limits<float>::max();
    std::optional<glm::vec3> hitPos;
    for (size_t i = 0; i < scene->mNumMeshes; ++i) {
        for (size_t j = 0; j < scene->mMeshes[i]->mNumFaces; ++j) {     
            if (scene->mMeshes[i]->mFaces[j].mNumIndices < 3)
                continue;

            std::array<glm::vec3, 3> v;
            for (size_t k = 0; k < 3; ++k) {
                v[k].x = scene->mMeshes[i]->mVertices[scene->mMeshes[i]->mFaces[j].mIndices[k]].x;
                v[k].y = scene->mMeshes[i]->mVertices[scene->mMeshes[i]->mFaces[j].mIndices[k]].y;
                v[k].z = scene->mMeshes[i]->mVertices[scene->mMeshes[i]->mFaces[j].mIndices[k]].z;
            }

            const auto&[n, p0] = trianglePlane(v);
            const auto p = intersectionLinePlane(n, p0, l, l0);
            if (p) {
                if(pointInTriangle(n, p0, v, *p)) {
                    const float dist = glm::length(*p - camPos);
                    if (!hitPos || dist < minDist) {
                        hitPos = *p;
                        minDist = dist;
                    }
                }
            }
        } 
    }

    if (hitPos) {
        std::cout << "Hit\n";
        s_draggingRot = true;
        s_dragStart = pos;
        s_entityStart = *hitPos;
        s_camStart = s_camera.getView();
    }
}

void CameraController::stopDraggingRot()
{
    s_entityStart = {};
    s_draggingRot = false;
}

void CameraController::drag(const glm::vec2& pos)
{
    if (s_draggingTrans) {
        const glm::vec2 v = -s_transFactor*(pos - s_dragStart);
        s_camera.setTransformation(glm::translate(s_camStart, {v.x, -v.y, 0.0}));
    }

    else if (s_draggingRot) {       
        ;
    }
}

void CameraController::zoom(const float factor)
{
    s_camera.translate({0.0f, 0.0f, 5*factor});
}

void CameraController::updateProjection()
{
    assert(Window::isInitialized() && "Window not initialized");

    // const auto params = calculateCameraIntrinsics(static_cast<float>(Window::getWidth()), static_cast<float>(Window::getHeight()), s_hFov);
    // const auto projection = glm::transpose(glm::mat4{
    //     glm::vec4{2.0f * params.intrinsics[0][0] / params.width,  0.0f                                           ,  1.0f - 2.0f * params.intrinsics[0][2] / params.width    , 0.0f                                          },
    //     glm::vec4{0.0f                                         , -2.0f * params.intrinsics[1][1] / params.height ,  2.0f * params.intrinsics[1][2] / params.height - 1.0f   , 0.0f                                          },
    //     glm::vec4{0.0f                                         ,  0.0f                                           ,  (s_zFar + s_zNear) / (s_zNear - s_zFar)                 , 2.0f * s_zFar * s_zNear / (s_zNear - s_zFar)  },
    //     glm::vec4{0.0f                                         ,  0.0f                                           , -1.0f                                                    , 0.0f                                          }
    // });

    // s_camera.setProjection(projection);

    float aspect = static_cast<float>(Window::getWidth()) / static_cast<float>(Window::getHeight());
    s_camera.setProjection(glm::perspective(s_hFov, aspect, s_zNear, s_zFar));
}