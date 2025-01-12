#include "pch.h"

#include "TriangulationUtils.h"
#include "../Components.h"

//------------------------------------------------------
//                      Bounding Box
//------------------------------------------------------

void updateBoundingBox(BoundingBoxComponent& component, const std::pair<glm::vec3, glm::vec3>& limits)
{
    const auto& [lower, upper] = limits;
    component.data->vertices[0] = {lower.x, lower.y, lower.z};
    component.data->vertices[1] = {upper.x, lower.y, lower.z};
    component.data->vertices[2] = {upper.x, upper.y, lower.z};
    component.data->vertices[3] = {lower.x, upper.y, lower.z};
    component.data->vertices[4] = {lower.x, lower.y, upper.z};
    component.data->vertices[5] = {upper.x, lower.y, upper.z};
    component.data->vertices[6] = {upper.x, upper.y, upper.z};
    component.data->vertices[7] = {lower.x, upper.y, upper.z};
}

bool rayIntersectsBoundingBox(const BoundingBoxComponent& component, const std::tuple<glm::vec3, glm::vec3>& ray_world)
{
    const auto& [v_ray_world, p_ray_world] = ray_world;

    bool hit = false;
    for (const auto& indices : component.data->indices) {
        std::array<glm::vec3, 3> p_vertices_world;
        for (size_t i = 0; i < 3; ++i)
            p_vertices_world[i] = component.data->vertices[indices[i]];

        const auto[n_tri_world, p_tri_world] = trianglePlane(p_vertices_world);
        glm::vec3 p_hitTmp_world;                               
        if (intersectionLinePlane(n_tri_world, p_tri_world, v_ray_world, p_ray_world, p_hitTmp_world) && 
            pointInTriangle(n_tri_world, p_tri_world, p_vertices_world, p_hitTmp_world)) {   

            hit = true;
            break;
        }
    }

    return hit;
}

//------------------------------------------------------
//                      Triangulation
//------------------------------------------------------

void updateTriangulation(TriangulationComponent& component, const glm::mat4& transform)
    {
        auto& [lower, upper] = component.limits;
        lower.x = lower.y = lower.z = std::numeric_limits<float>::max();
        upper.x = upper.y = upper.z = std::numeric_limits<float>::min();

        if (component.updatedVertices.size() != component.data->vertices.size()) 
            component.updatedVertices.resize(component.data->vertices.size());
            
        for (size_t i = 0; i < component.updatedVertices.size(); ++i) {
            component.updatedVertices[i] = glm::vec4(component.data->vertices[i], 1.0f) * transform;
            lower.x = std::min(lower.x, component.updatedVertices[i].x);
            lower.y = std::min(lower.y, component.updatedVertices[i].y);
            lower.z = std::min(lower.z, component.updatedVertices[i].z);
            upper.x = std::max(upper.x, component.updatedVertices[i].x);
            upper.y = std::max(upper.y, component.updatedVertices[i].y);
            upper.z = std::max(upper.z, component.updatedVertices[i].z);      
        }
    }

bool rayTriangulationIntersection(const TriangulationComponent& component, const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist)
{
    const auto& [v_ray_world, p_ray_world] = ray_world;

    minDist = std::numeric_limits<float>::max();
    bool hit = false;
    for (const auto& indices : component.data->indices) {
        std::array<glm::vec3, 3> p_vertices_world;
        for (size_t i = 0; i < 3; ++i)
            p_vertices_world[i] = component.updatedVertices[indices[i]];

        const auto[n_tri_world, p_tri_world] = trianglePlane(p_vertices_world);
        glm::vec3 p_hitTmp_world;                               
        if (intersectionLinePlane(n_tri_world, p_tri_world, v_ray_world, p_ray_world, p_hitTmp_world) && 
            pointInTriangle(n_tri_world, p_tri_world, p_vertices_world, p_hitTmp_world)) {  

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