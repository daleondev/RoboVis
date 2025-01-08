#pragma once

#include "Entity.h"

#include "Renderer/VertexArray.h"

class Sphere : public EntityOld {

public:
    Sphere(const glm::vec4& color);
    virtual ~Sphere();

    virtual void draw(const Camera& camera) override;
    virtual void updateTriangulationData() override { }

    inline virtual bool rayIntersection(const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist) const override { return false; }

private:   
    void createBuffers();

    glm::vec4 m_color;
    VertexArray m_vertexArray;

};
