#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Camera.h"

struct TriangulationData
{
    std::vector<glm::vec3> vertices;
    std::vector<std::array<uint16_t, 3>> indices;
};

class Entity {

public:
    Entity();
    virtual ~Entity();

    virtual void draw(const Camera& camera) = 0;
    virtual void updateTriangulationData() = 0;

    virtual bool rayIntersection(const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist) const;

    void reset();
    void setTranslation(const glm::vec3& p_world);
    void setRotation(const float angle, const glm::vec3& v_axis_world);
    void setTransformation(const glm::mat4& t_ent_world);

    void translateWorld(const glm::vec3& v_world);
    void translate(const glm::vec3& v_ent);
    void rotate(const float angle, const glm::vec3& v_axis_ent);
    void rotate(const glm::mat3& r_ent);
    void transform(const glm::mat4& t_ent); 
    void scale(const glm::vec3& scale);

    inline glm::mat4 getModel() const { return m_model; }
    inline glm::mat4 getPos() const { return getModel(); }

    inline std::shared_ptr<TriangulationData> getTriangulationData() const { return m_triData; }

    inline bool isVisible() const { return m_visible; }
    inline void setVisible(const bool visible) { m_visible = visible; }

protected:    
    virtual void createBuffers() = 0;

    void updateMvp(const Camera& camera);

    glm::mat4 m_model;
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<TriangulationData> m_triData;
    bool m_visible;

};
