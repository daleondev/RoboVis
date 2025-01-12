#pragma once

class Entity;

//------------------------------------------------------
//                      Joint
//------------------------------------------------------

struct JointComponent;
enum class JointType;

glm::mat4 forwardTransformJoint(JointComponent& component);

void destroyJoint(JointComponent& component, const bool destroyParent = true);
void recurseJoint(JointComponent& component, const std::function<void(Entity)>& func, const bool execForParent = true);

const char* jointTypeToStr(const JointType type);
JointType jointStrToType(const char* str);

//------------------------------------------------------
//                      Robot
//------------------------------------------------------

struct RobotComponent;
struct Trajectory;
class Timestep;

void setupRobot(RobotComponent& component);
void updateRobot(RobotComponent& component, Entity entity, const Timestep dt);
glm::mat4 forwardTransformRobot(const Entity entity);
void destroyRobot(RobotComponent& component);