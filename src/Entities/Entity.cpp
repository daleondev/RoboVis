#include "Entity.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Entity::Entity(const std::shared_ptr<Shader>& shader)
    : m_shader(shader), m_model{ glm::mat4(1.0f) }
{
    
}

Entity::~Entity()
{
    m_vertexArray.release();
    m_shader->release();
}

void Entity::reset()
{
    m_model = glm::mat4(1.0f);
}

void Entity::setTranslation(const glm::vec3& translation)
{
    for (size_t i = 0; i < 3; ++i)
        m_model[3][i] = translation[i];
}

void Entity::setRotation(const float angle, const glm::vec3& axis)
{
    const glm::mat3 rot = glm::toMat3(glm::angleAxis(angle, axis));
    for (size_t i = 0; i < 3; ++i)
        for (size_t j = 0; j < 3; ++j)
            m_model[i][j] = rot[i][j];
}

void Entity::setTransformation(const glm::mat4& transformation)
{
    m_model = transformation;
}

void Entity::translate(const glm::vec3& translation)
{
    m_model = glm::translate(m_model, translation);
}

void Entity::rotate(const float angle, const glm::vec3& axis)
{
    m_model = glm::rotate(m_model, angle, axis);
}

void Entity::transform(const glm::mat4& transformation)
{
    m_model *= transformation;
} 

void Entity::scale(const glm::vec3& scale)
{
    m_model = glm::scale(m_model, scale);
}

void Entity::updateMvp(const std::optional<Camera>& camera)
{
    glm::mat4 mvp = m_model;
    if (camera)
        mvp = camera->getProjection() * camera->getView() * m_model;

    m_shader->bind();
    m_shader->uploadMat4("u_mvp", mvp);
}