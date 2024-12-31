#pragma once

#include "Timestep.h"

#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"

class Camera {

public:
    Camera();
    ~Camera();

    void setProjection(const glm::mat4& projection);

    void reset();

    void setTranslation(const glm::vec3& p_world);
    void setRotation(const float angle, const glm::vec3& l_axis_world);
    void setTransformation(const glm::mat4& t_cam_world);

    void translateWorld(const glm::vec3& l_world);
    void translate(const glm::vec3& l_cam);
    void rotate(const float angle, const glm::vec3& l_axis_cam);
    void rotate(const glm::mat3& R);
    void transform(const glm::mat4& transformation);  
    void reorthogonalize();

    inline glm::mat4 getPosition() const { return m_pos; };
    inline glm::mat4 getProjection() const { return m_projection; };
    inline glm::mat4 getView() const { return m_view; };

private:
    void posToView();

    glm::mat4 m_pos;
    glm::mat4 m_projection;
    glm::mat4 m_view;
};

class CameraController 
{
public:
    static void init(const float hFov, const float zNear = 0.3f, const float zFar = 1000.0f, const glm::mat4& t_camInit_world = glm::mat4(1.0f));

    static void onUpdate(const Timestep ts);
    static void onResize();

    static void stopInteraction();
    static void startDraggingTrans(const glm::vec2& p_mouse_screen);
    static void stopDraggingTrans();
    static void startDraggingRot(const glm::vec2& p_mouse_screen);
    static void stopDraggingRot();
    static void drag(const glm::vec2& p_mouse_screen);
    static void zoom(const float factor);

    inline static Camera& getCamera() { return s_camera; }
    inline static bool isDragging() { return s_draggingTrans || s_draggingRot; }
    inline static std::optional<glm::vec3> getDraggingPosition() { return s_dragPos; }
private:
    static void updateProjection();
    static std::tuple<glm::vec3, glm::vec3> screenToWorld(const glm::vec2& p_mouse_screen, const glm::mat4& t_cam_world);
    static std::tuple<glm::vec3, glm::vec3> screenToCam(const glm::vec2& p_mouse_screen);
    static std::tuple<glm::vec3, glm::vec3> cameraRay(const glm::vec2& p_mouse_screen, const glm::mat4& t_cam_world);

    static Camera s_camera;  
    static float s_hFov;
    static float s_zNear;
    static float s_zFar;
    static glm::mat4 s_initialTransformation;
    
    static bool s_draggingTrans;
    static bool s_draggingRot;
    static glm::vec2 s_screenPosPrev;   
    static glm::mat4 s_camPosPrev;
    static std::optional<glm::vec3> s_dragPos;

    inline static const float s_scrollFactor = 5.0f;
    inline static const float s_dragFactor = 0.01f;
    inline static const float s_rotFactor = 0.2f;

    
};