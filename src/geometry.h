#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/epsilon.hpp>

#include <tuple>
#include <optional>
#include <iostream>

static float deg2rad(const float deg) 
{
    return deg * static_cast<float>(M_PI) / 180.0f;
};

static glm::vec4 extendedCross(const glm::vec4& A, const glm::vec4& B, const glm::vec4& C)
{
    //              --                           --
    //              | (-1)^0 * DET[A^0, B^0, C^0] |
    // X(A, B, C) = | (-1)^1 * DET[A^1, B^1, C^1] |
    //              | (-1)^2 * DET[A^0, B^2, C^2] |
    //              | (-1)^3 * DET[A^3, B^3, C^3] |
    //              --                           --
    return glm::vec4{
        std::pow(-1, 0) * glm::determinant(glm::mat3( {A[1], B[1], C[1]}, {A[2], B[2], C[2]}, {A[3], B[3], C[3]} )),
        std::pow(-1, 1) * glm::determinant(glm::mat3( {A[0], B[0], C[0]}, {A[2], B[2], C[2]}, {A[3], B[3], C[3]} )),
        std::pow(-1, 2) * glm::determinant(glm::mat3( {A[0], B[0], C[0]}, {A[1], B[1], C[1]}, {A[3], B[3], C[3]} )),
        std::pow(-1, 3) * glm::determinant(glm::mat3( {A[0], B[0], C[0]}, {A[1], B[1], C[1]}, {A[2], B[2], C[2]} ))
    };
}

static glm::mat3 skewSymmetricCross(const glm::vec3& v)
{
    return glm::mat3(
        {  0.0f, -v[2],  v[1] },
        {  v[2],  0.0f, -v[0] },
        { -v[1],  v[0],  0.0f }
    );
}

static glm::mat3 rotationVecToVec(const glm::vec3& a, const glm::vec3& b)
{
    // https://math.stackexchange.com/a/476311
    const auto v = glm::cross(a, b);
    const auto s = glm::length(v);
    const auto c = dot(a,b);

    const auto vx = skewSymmetricCross(v);
    const auto r = glm::mat3(1.0f) + vx + vx*vx * (1/(1+c));
    return r;
}

static std::tuple<glm::vec3, glm::vec3> trianglePlane(const std::array<glm::vec3, 3>& v)
{  
    auto N = extendedCross(glm::vec4(v[0], 1.0f), glm::vec4(v[1], 1.0f), glm::vec4(v[2], 1.0f));
    const glm::vec3 n = glm::normalize(glm::vec3(N/N.w));

    const glm::vec3 p0 = (v[0]+v[1]+v[2])/3.0f;

    return {n, p0};
}

static std::optional<glm::vec3> intersectionLinePlane(const glm::vec3& n, const glm::vec3& p0, const glm::vec3& l, const glm::vec3& l0)
{
    // https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
    const float numerator = glm::dot(p0-l0, n);
    if (numerator == 0)
        return {};

    const float denominator = glm::dot(l, n);
    const float d = numerator/denominator;
    const glm::vec3 p = l0+l*d;

    return p;
}

static int16_t location(const glm::vec3& N, const glm::vec3& P)
{
    const float l = dot(N, P);
    if (l < 0)
        return -1;
    else if (l > 0)
        return 1;     
    return 0;
}

static void rotateAroundPoint(const glm::vec3& p0, const glm::mat3& r, glm::vec3& p)
{
    glm::mat4 mat(1.0f);
    mat[0][3] = p[0] - p0[0];
    mat[1][3] = p[1] - p0[1];
    mat[2][3] = p[2] - p0[2];

    mat *= glm::mat4(r);
    p[0] = mat[0][3];
    p[1] = mat[1][3];
    p[2] = mat[2][3];
}

static bool pointInTriangle(const glm::vec3& n, const glm::vec3& p0, std::array<glm::vec3, 3> v, glm::vec3 p)
{
    constexpr glm::vec3 UNIT_Z(0.0f, 0.0f, 1.0f);

    if (glm::any(glm::isnan(n)) || glm::any(glm::isnan(p0)))
        return false;

    glm::mat3 r(1.0f);
    if (!glm::all(glm::epsilonEqual(n, UNIT_Z, glm::epsilon<float>())) && !glm::all(glm::epsilonEqual(n, -UNIT_Z, glm::epsilon<float>())))
        r = rotationVecToVec(n, UNIT_Z);

    rotateAroundPoint(p0, r, p);

    std::array<glm::vec3, 3> V, V_;
    for (size_t i = 0; i < 3; ++i) {
        rotateAroundPoint(p0, r, v[i]);
        V[i] = glm::vec3(glm::vec2(v[i]), 1.0f);
        V_[i] = glm::vec3(glm::vec2(v[i]), 0.0f);
    }

    const auto P = glm::vec3(glm::vec2(p), 1.0f);
    int16_t locPrev;
    size_t j = 1;
    for (size_t i = 0; i < 3; ++i) {
        if(j > 2)
            j = 0;

        const auto N = glm::cross(V[i], V_[j++] - V_[i]);
        const auto loc = location(N, P);

        if (loc == 0)
            return true;
            
        if (i > 0 && loc != locPrev)
            return false;

        locPrev = loc;
    }

    return true;
}

// static std::optional<glm::vec3> rayMeshIntersection(const std::array<glm::vec3, 3>& v, const glm::vec3& l, const glm::vec3& l0)
// {

// }

static float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}