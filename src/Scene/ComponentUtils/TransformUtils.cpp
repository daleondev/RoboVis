#include "pch.h"

#include "TransformUtils.h"
#include "../Components.h"

void setTransform(TransformComponent& component, const glm::mat4& transform)
{
    component.translation = getMat4Translation(transform);

    component.rotation = toEulerZYX(getMat4Rotation(transform));

    component.scale.x = glm::length(getMat4AxisX(transform));
    component.scale.y = glm::length(getMat4AxisY(transform));
    component.scale.z = glm::length(getMat4AxisZ(transform));
}

glm::mat4 getTransform(const TransformComponent& component)
{
    glm::mat4 transform(1.0f);
    setMat4Translation(transform, component.translation);
    setMat4Rotation(transform, eulerZYX(component.rotation));
    return glm::transpose(glm::scale(glm::transpose(transform), component.scale));
}

void setTransformChangedCallback(TransformComponent& component, const std::function<void(const glm::mat4&)>& callback) 
{ 
    component.changedCallback = callback; 
}

void detectTransformChange(TransformComponent& component) 
{ 
    (glm::vec3&)component.translationListener = component.translation;
    (glm::vec3&)component.rotationListener = component.rotation;
    (glm::vec3&)component.scaleListener = component.scale;
    if (component.changedCallback && (component.translationListener().edge() || component.rotationListener().edge() || component.scaleListener().edge())) 
        (*component.changedCallback)(getTransform(component)); 
}