#include "pch.h"

#include "Entity.h"

#include "Util/Log.h"
#include "Util/geometry.h"

Entity::Entity()
    : m_shader(nullptr), m_model(1.0f), m_visible(true)
{
}

Entity::~Entity()
{
    m_shader->release();
}

bool Entity::rayIntersection(const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist) const
{
    const auto& [v_ray_world, p_ray_world] = ray_world;

    minDist = std::numeric_limits<float>::max();
    bool hit = false;
    for (const auto& indices : m_triData->indices) {
        std::array<glm::vec3, 3> p_vertices_world;
        for (size_t i = 0; i < 3; ++i)
            p_vertices_world[i] = m_triData->vertices[indices[i]];

        const auto[n_tri_world, p_tri_world] = trianglePlane(p_vertices_world);
        glm::vec3 p_hitTmp_world;                               
        if (intersectionLinePlane(n_tri_world, p_tri_world, v_ray_world, p_ray_world, p_hitTmp_world) && pointInTriangle(n_tri_world, p_tri_world, p_vertices_world, p_hitTmp_world)) {                                                         
            const float dist = glm::length(p_hitTmp_world - p_ray_world);

            if (!hit || dist < minDist) {
                hit = true;
                p_hit_world = p_hitTmp_world;
                minDist = dist;
            }
        }
    }

    return hit;
}

void Entity::reset()
{
    m_model = glm::mat4(1.0f);
}

void Entity::setTranslation(const glm::vec3& p_world)
{
    setMat4Translation(m_model, p_world);
    updateTriangulationData();
}

void Entity::setRotation(const float angle, const glm::vec3& v_axis_world)
{
    const glm::mat3 r_world = angleAxisF(angle, v_axis_world);
    setMat4Rotation(m_model, r_world);
    updateTriangulationData();
}

void Entity::setTransformation(const glm::mat4& t_ent_world)
{
    m_model = t_ent_world;
    updateTriangulationData();
}

void Entity::translateWorld(const glm::vec3& v_world)
{
    for (size_t i = 0; i < 3; ++i)
        m_model[i][3] += v_world[i];
    updateTriangulationData();
}

void Entity:: translate(const glm::vec3& v_ent)
{
    const auto v_world = getMat4AxisX(m_model)*v_ent.x + getMat4AxisY(m_model)*v_ent.y + getMat4AxisZ(m_model)*v_ent.z;
    translateWorld(v_world);
}

void Entity::rotate(const float angle, const glm::vec3& v_axis_ent)
{
    const auto r_ent = angleAxisF(angle, v_axis_ent);
    m_model = glm::mat4(r_ent) * m_model;
    updateTriangulationData();
}

void Entity::rotate(const glm::mat3& r_ent)
{
    m_model = glm::mat4(r_ent) * m_model;
    updateTriangulationData();
}

void Entity::transform(const glm::mat4& t_ent)
{
    m_model = t_ent * m_model;
    updateTriangulationData();
} 

void Entity::scale(const glm::vec3& scale)
{
    m_model = glm::transpose(glm::scale(glm::transpose(m_model), scale));
    updateTriangulationData();
}

void Entity::updateMvp(const Camera& camera)
{
    glm::mat4 mvp = camera.getProjection() * camera.getView() * glm::transpose(m_model);

    m_shader->bind();
    m_shader->uploadMat4("u_mvp", mvp);
}