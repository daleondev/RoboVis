#pragma once

#include "Timestep.h"

#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"

#include <GLES3/gl3.h>

#include <glm/glm.hpp>

struct CameraParameters {
    float width;
    float height;
    float hfov;
    glm::mat3 intrinsics;
};

class Camera {

public:
    Camera();
    ~Camera();

    void setProjection(const glm::mat4& projection);

    void reset();

    void setTranslation(const glm::vec3& translation);
    void setRotation(const float angle, const glm::vec3& axis);
    void setTransformation(const glm::mat4& transformation);

    void translate(const glm::vec3& translation);
    void rotate(const float angle, const glm::vec3& axis);
    void transform(const glm::mat4& transformation);  

    glm::mat4 getProjection() const;
    glm::mat4 getView() const;

private:
    glm::mat4 m_projection;
    glm::mat4 m_view;
};

class CameraController 
{
public:
    static void init(const float hFov, const float zNear = 0.3f, const float zFar = 1000.0f, const glm::mat4& initialTransformation = glm::mat4(1.0f));

    static void onUpdate(const Timestep ts);
    static void onEvent(Event& e);

    static Camera& getCamera() { return s_camera; }
private:
    static void updateProjection();

    static bool onMouseScrolled(MouseScrolledEvent& e);
    static bool onWindowResized(WindowResizeEvent& e);

    static Camera s_camera;
    static float s_hFov;
    static float s_zNear;
    static float s_zFar;

    inline static float s_translationSpeed = 500.0f;
    inline static float s_rotationSpeed = 180.0f;

    
};