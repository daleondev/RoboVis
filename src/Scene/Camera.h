#pragma once

#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"

#include "Util/Timestep.h"

class Camera {

public:
    Camera();
    ~Camera();

    void setProjection(const float hFov, const float m_zNear, const float m_zFar, const uint16_t width, const uint16_t height);

    void reset();

    void setTranslation(const glm::vec3& p_world);
    void setRotation(const float angle, const glm::vec3& v_axis_world);
    void setTransformation(const glm::mat4& t_cam_world);

    void translateWorld(const glm::vec3& v_world);
    void translate(const glm::vec3& v_cam);
    void rotate(const float angle, const glm::vec3& v_axis_cam);
    void rotate(const glm::mat3& r_cam);
    void transform(const glm::mat4& t_cam);
    
    std::tuple<glm::vec3, glm::vec3> screenToCam(const glm::vec2& p_mouse_screen) const;
    std::tuple<glm::vec3, glm::vec3> screenToWorld(const glm::vec2& p_mouse_screen) const;  
    std::tuple<glm::vec3, glm::vec3> ray(const glm::vec2& p_mouse_screen) const;

    inline glm::mat4 getTransformation() const { return m_transform; };
    inline glm::mat4 getProjection() const { return m_projection; };
    inline glm::mat4 getView() const { return m_view; };

private:
    void posToView();

    glm::mat4 m_transform;
    glm::mat4 m_projection;
    glm::mat4 m_view;

    float m_hFov;
    float m_zNear;
    float m_zFar;
};

class CameraController 
{
public:
    static void init(const float hFov, const float zNear = 0.3f, const float zFar = 1000.0f, const glm::mat4& t_camInit_world = glm::mat4(1.0f));

    static void update(const Timestep ts);
    static void onResize();

    static void stopInteraction();
    static void startDraggingTrans(const glm::vec2& p_mouse_screen);
    static void stopDraggingTrans();
    static void startDraggingRot(const glm::vec2& p_mouse_screen);
    static void stopDraggingRot();
    static void startDragging(const glm::vec2& p_mouse_screen);
    static void drag(const glm::vec2& p_mouse_screen);
    static void zoom(const float factor);

    inline static Camera& getCamera() { return s_camera; }
    inline static bool isDragging() { return s_draggingTrans || s_draggingRot; }
    inline static glm::vec3 getDraggingPosition() { return s_dragPos; }
private:
    static void updateProjection();
    
    static Camera s_camera;  
    static float s_hFov;
    static float s_zNear;
    static float s_zFar;
    static glm::mat4 s_initialTransformation;
    
    static bool s_draggingTrans;
    static bool s_draggingRot;
    static glm::vec2 s_screenPosPrev;   
    static glm::mat4 s_camPosPrev;
    static glm::vec3 s_dragPos;

    inline static constexpr float s_scrollFactor = 1.0f;
    inline static constexpr float s_dragFactor = 0.001f;
    inline static constexpr float s_rotFactor = 0.2f;
    
};