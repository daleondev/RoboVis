#pragma once

struct TransformComponent;

void setTransform(TransformComponent& component, const glm::mat4& transform);
glm::mat4 getTransform(const TransformComponent& component);

void setTransformChangedCallback(TransformComponent& component, const std::function<void(const glm::mat4&)>& callback);
void detectTransformChange(TransformComponent& component);