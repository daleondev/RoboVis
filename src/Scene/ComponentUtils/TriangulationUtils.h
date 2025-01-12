#pragma once

struct BoundingBoxComponent;
struct TriangulationComponent;

//------------------------------------------------------
//                      Bounding Box
//------------------------------------------------------

void updateBoundingBox(BoundingBoxComponent& component, const std::pair<glm::vec3, glm::vec3>& limits);
bool rayIntersectsBoundingBox(const BoundingBoxComponent& component, const std::tuple<glm::vec3, glm::vec3>& ray_world);

//------------------------------------------------------
//                      Triangulation
//------------------------------------------------------

void updateTriangulation(TriangulationComponent& component, const glm::mat4& transform);
bool rayTriangulationIntersection(const TriangulationComponent& component, const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist);