#include "pch.h"

#include "RobotUtils.h"
#include "../Components.h"

//------------------------------------------------------
//                      Joint
//------------------------------------------------------

glm::mat4 forwardTransformJoint(JointComponent& component)
{
    component.value = std::clamp(component.value, component.limits[0], component.limits[1]);
    const glm::mat4 t_parent_world = component.parentLink.getComponent<TransformComponent>();
    const auto t_child_world = glm::mat4(angleAxisF(component.value, component.rotationAxis)) * component.parentToChild * t_parent_world;
    setTransform(component.childLink.getComponent<TransformComponent>(), t_child_world);

    if (component.childLink.hasComponent<JointComponent>())
        return forwardTransformJoint(component.childLink.getComponent<JointComponent>());
    else
        return t_child_world;
}

void destroyJoint(JointComponent& component, const bool destroyParent)
{
    if (component.childLink.hasComponent<JointComponent>())
        destroyJoint(component.childLink.getComponent<JointComponent>(), false);
    Scene::destroyEntity(component.childLink);
    if (destroyParent)
        Scene::destroyEntity(component.parentLink);
}

void recurseJoint(JointComponent& component, const std::function<void(Entity)>& func, const bool execForParent)
{
    if (component.childLink.hasComponent<JointComponent>())
        recurseJoint(component.childLink.getComponent<JointComponent>(), func, false);
    func(component.childLink);  
    if (execForParent)
        func(component.parentLink);      
}

const char* jointTypeToStr(const JointType type)
{
    switch (type) {
        case JointType::Fixed: return "fixed";
        case JointType::Revolute: return "revolute";
        default: return "invalid";
    }
}

JointType jointStrToType(const char* str)
{
    if (const_strcmp(str, "fixed"))
        return JointType::Fixed;
    else if (const_strcmp(str, "revolute"))
        return JointType::Revolute;
    return JointType::Invalid;
}

//------------------------------------------------------
//                      Robot
//------------------------------------------------------

void setupRobot(RobotComponent& component)
{
    if (component.baseLink.hasComponent<JointComponent>()) {
        recurseJoint(component.baseLink.getComponent<JointComponent>(), [&component](Entity entity) -> void {
            if (entity.hasComponent<JointComponent>())
                component.joints.push_back(entity.getComponent<JointComponent>());
        });
    }
    std::ranges::reverse(component.joints);
}

void updateRobot(RobotComponent& component, Entity entity, const Timestep dt)
{
    if (entity.hasComponent<JointComponent>()) {
        auto& joint = entity.getComponent<JointComponent>();
        forwardTransformJoint(joint);

        // properties
        auto& properties = entity.getComponent<PropertiesComponent>();
        if ((bool&)component.visible = properties.visible; component.visible().edge()) {
            recurseJoint(joint, [&component](Entity entity) -> void {
                if (entity.hasComponent<PropertiesComponent>())
                    entity.getComponent<PropertiesComponent>().visible = component.visible;
            });
        }
        if ((bool&)component.clickable = properties.clickable; component.clickable().edge()) {
            recurseJoint(joint, [&component](Entity entity) -> void {
                if (entity.hasComponent<PropertiesComponent>())
                    entity.getComponent<PropertiesComponent>().clickable = component.clickable;
            });
        }

        // frames
        if (component.drawFrames().edge()) {
            recurseJoint(joint, [&component](Entity entity) -> void {
                if (entity.hasComponent<FrameRendererComponent>())
                    entity.getComponent<FrameRendererComponent>().internalVisible = component.drawFrames;
            });
        }
        if (component.frameScale().edge()) {
            recurseJoint(joint, [&component](Entity entity) -> void {
                if (entity.hasComponent<FrameRendererComponent>() && entity.getComponent<FrameRendererComponent>().internalTransform) {
                    auto& t = *entity.getComponent<FrameRendererComponent>().internalTransform;
                    t = rescaleMat4(t) * glm::transpose(glm::scale(glm::transpose(glm::mat4(1.0f)), {component.frameScale, component.frameScale, component.frameScale}));
                    if (entity.hasComponent<TriangulationComponent>())
                        updateTriangulation(entity.getComponent<TriangulationComponent>(), t);
                }                   
            });
        }

        // bounding box
        if (component.drawBoundingBoxes().edge()) {
            recurseJoint(joint, [&component](Entity entity) -> void {
                if (entity.hasComponent<BoundingBoxComponent>())
                    entity.getComponent<BoundingBoxComponent>().visible = component.drawBoundingBoxes;
            });
        }
        if (component.boundingBoxColor().edge()) {
            recurseJoint(joint, [&component](Entity entity) -> void {
                if (entity.hasComponent<BoundingBoxComponent>())
                    entity.getComponent<BoundingBoxComponent>().color = component.boundingBoxColor;        
            });
        }

        // trajectory
        if (component.trajectory) {
            auto& [active, currentTime, currentIndex, jointValues, times] = *component.trajectory; 

            while (currentTime > times[currentIndex] && currentIndex < jointValues.size())
                currentIndex++;
            while (currentTime < times[currentIndex-1] && currentIndex > 0)
                currentIndex--;

            if (active && currentTime < times.back()) {
                currentTime += dt;

                if (currentIndex > 0 && currentIndex < jointValues.size())
                    for (size_t i = 0; i < component.joints.size(); ++i) {           
                        component.joints[i].get().value = map(
                            currentTime, 
                            times[currentIndex-1],          times[currentIndex], 
                            jointValues[currentIndex-1][i], jointValues[currentIndex][i]); 
                    } 
            }
            else if (active)
                active = false;
        }
    }
}

glm::mat4 forwardTransformRobot(const Entity entity)
{
    return entity.hasComponent<JointComponent>() ? forwardTransformJoint(entity.getComponent<JointComponent>()) : glm::mat4(1.0f);
}

void destroyRobot(RobotComponent& component)
{
    if (component.baseLink.hasComponent<JointComponent>()) 
        destroyJoint(component.baseLink.getComponent<JointComponent>());
    Scene::destroyEntity(component.baseLink);
}