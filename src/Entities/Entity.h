#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Camera.h"

class Camera;

class Entity {

public:
    Entity(const std::shared_ptr<Shader>& shader);
    virtual ~Entity();

    virtual void draw(const std::optional<Camera>& camera) = 0;

    void reset();
    void setTranslation(const glm::vec3& translation);
    void setRotation(const float angle, const glm::vec3& axis);
    void setTransformation(const glm::mat4& transformation);

    void translate(const glm::vec3& translation);
    void rotate(const float angle, const glm::vec3& axis);
    void transform(const glm::mat4& transformation); 
    void scale(const glm::vec3& scale);

    inline bool isVisible() const { return m_visible; }
    inline void setVisible(const bool visible) { m_visible = visible; }

    inline glm::mat4 getModel() const { return m_model; }
    inline glm::mat4 getPos() const { return getModel(); }

protected:
    virtual void createBuffers() = 0;

    void updateMvp(const std::optional<Camera>& camera);

    glm::mat4 m_model;
    std::shared_ptr<Shader> m_shader;
    bool m_visible;

};
