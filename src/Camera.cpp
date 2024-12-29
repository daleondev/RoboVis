#include "Camera.h"

#include "Window.h"
#include "Input.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

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

static CameraParameters calculateCameraIntrinsics(const float width, const float height, const float hfov)
{
    const auto deg2rad = [](const float deg)->float {
        return deg * static_cast<float>(M_PI) / 180.0f;
    };

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

    s_camera.setTransformation(initialTransformation);

    updateProjection();
}

void CameraController::onUpdate(const Timestep dt)
{
    if (Input::isKeyPressed(GLFW_KEY_A))
        s_camera.translate({-s_translationSpeed * dt, 0.0f, 0.0f});

    else if(Input::isKeyPressed(GLFW_KEY_D))
        s_camera.translate({s_translationSpeed * dt, 0.0f, 0.0f});

    if (Input::isKeyPressed(GLFW_KEY_W))
        s_camera.translate({0.0f, s_translationSpeed * dt, 0.0f});

    else if (Input::isKeyPressed(GLFW_KEY_S))
        s_camera.translate({0.0f, -s_translationSpeed * dt, 0.0f});
}

void CameraController::onEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<MouseScrolledEvent>(CameraController::onMouseScrolled);
    dispatcher.dispatch<WindowResizeEvent>(CameraController::onWindowResized);
}

void CameraController::updateProjection()
{
    assert(Window::isInitialized() && "Window not initialized");

    const auto params = calculateCameraIntrinsics(static_cast<float>(Window::getWidth()), static_cast<float>(Window::getHeight()), s_hFov);
    const auto projection = glm::transpose(glm::mat4{
        glm::vec4{2.0f * params.intrinsics[0][0] / params.width,  0.0f                                           ,  1.0f - 2.0f * params.intrinsics[0][2] / params.width    , 0.0f                                          },
        glm::vec4{0.0f                                         , -2.0f * params.intrinsics[1][1] / params.height ,  2.0f * params.intrinsics[1][2] / params.height - 1.0f   , 0.0f                                          },
        glm::vec4{0.0f                                         ,  0.0f                                           ,  (s_zFar + s_zNear) / (s_zNear - s_zFar)                 , 2.0f * s_zFar * s_zNear / (s_zNear - s_zFar)  },
        glm::vec4{0.0f                                         ,  0.0f                                           , -1.0f                                                    , 0.0f                                          }
    });

    s_camera.setProjection(projection);
}

bool CameraController::onMouseScrolled(MouseScrolledEvent& e)
{
    // m_zoomLevel -= e.getYOffset() * 0.5f;
    // m_zoomLevel = std::max(m_zoomLevel, 0.25f);
    // m_camera.setProjection(-m_aspectRatio * m_zoomLevel, +m_aspectRatio * m_zoomLevel, -m_zoomLevel, +m_zoomLevel);

    return false;
}

bool CameraController::onWindowResized(WindowResizeEvent& e)
{
    updateProjection();

    return false;
}
