#pragma once

#include "util.h"

#include "Log.h"

#define MAT_ROW3(mat, row)  glm::vec3(  mat[row][0], mat[row][1], mat[row][2])
#define MAT_COL3(mat, col)  glm::vec3(  mat[0][col], mat[1][col], mat[2][col])
#define MAT3(mat, row, col) glm::mat3(  mat[row+0][col+0], mat[row+0][col+1], mat[row+0][col+2],\
                                        mat[row+1][col+0], mat[row+1][col+1], mat[row+1][col+2],\
                                        mat[row+2][col+0], mat[row+2][col+1], mat[row+2][col+2])

template<typename T>
static void printMat4(const T& mat)
{
    std::stringstream ss;
    ss << '\n';
    for (size_t i = 0; i < 4; ++i) {
        ss << strPrintf("%5.5f %-5.5f %-5.5f %-5.5f\n", mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
    }
    LOG_TRACE << ss.str();
}

template<typename T>
static void printMat3(const T& mat)
{
    std::stringstream ss;
    ss << '\n';
    for (size_t i = 0; i < 3; ++i) {
        ss << strPrintf("%5.5f %-5.5f %-5.5f\n", mat[i][0], mat[i][1], mat[i][2]);
    }
    LOG_TRACE << ss.str();
}

template<typename T>
static void printVec4(const T& vec)
{
    LOG_TRACE << strPrintf("%5.5f %-5.5f %-5.5f %-5.5f\n", vec[0], vec[1], vec[2], vec[3]);
}

template<typename T>
static void printVec3(const T& vec)
{
    LOG_TRACE << strPrintf("%5.5f %-5.5f %-5.5f\n", vec[0], vec[1], vec[2]);
}

template<typename TS, typename TD>
static TD convertVec2(const TS& src)
{
    TD dest;
    for (int i = 0; i < 2; ++i)
        dest[i] = src[i];
    return dest;
}

template<typename TS, typename TD>
static TD convertVec3(const TS& src)
{
    TD dest;
    for (int i = 0; i < 3; ++i)
        dest[i] = src[i];
    return dest;
}

template<typename TS, typename TD>
static TD convertVec4(const TS& src)
{
    TD dest;
    for (int i = 0; i < 4; ++i)
        dest[i] = src[i];
    return dest;
}

template<typename TS, typename TD>
static TD convertMat3(const TS& src)
{
    TD dest;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            dest[i][j] = src[i][j];
    return dest;
}

template<typename TS, typename TD>
static TD convertMat4(const TS& src)
{
    TD dest;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            dest[i][j] = src[i][j];
    return dest;
}

static float deg2rad(const float deg) 
{
    return deg * static_cast<float>(M_PI) / 180.0f;
};

static float rad2deg(const float rad) 
{
    return rad * 180.0f / static_cast<float>(M_PI);
};

static glm::mat4& setMat4Translation(glm::mat4& mat, const glm::vec3& p)
{
    for (size_t i = 0; i < 3; ++i)
        mat[i][3] = p[i];
    return mat;
}

static glm::mat4& setMat4Rotation(glm::mat4& mat, const glm::mat3& r)
{
    for (size_t i = 0; i < 3; ++i)
        for (size_t j = 0; j < 3; ++j)
            mat[i][j] = r[i][j];
    return mat;
}


static glm::vec3 getMat4Translation(const glm::mat4& mat)
{
    return MAT_COL3(mat, 3);
}

static glm::vec3 getMat4AxisZ(const glm::mat4& mat)
{
    return MAT_COL3(mat, 2);
}

static glm::vec3 getMat4AxisY(const glm::mat4& mat)
{
    return MAT_COL3(mat, 1);
}

static glm::vec3 getMat4AxisX(const glm::mat4& mat)
{
    return MAT_COL3(mat, 0);
}

static glm::mat3 getMat4Rotation(const glm::mat4& mat)
{
    return MAT3(mat, 0, 0);
}

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

static std::tuple<glm::vec3, glm::vec3> trianglePlane(const std::array<glm::vec3, 3>& p_tri)
{  
    auto N_plane = extendedCross(glm::vec4(p_tri[0], 1.0f), glm::vec4(p_tri[1], 1.0f), glm::vec4(p_tri[2], 1.0f));
    if (N_plane.w != 0)
        N_plane /= N_plane.w;
    const glm::vec3 n_plane = glm::normalize(N_plane);
    const glm::vec3 p_plane = (p_tri[0]+p_tri[1]+p_tri[2])/3.0f;
    return {n_plane, p_plane};
}

static bool intersectionLinePlane(const glm::vec3& n_plane, const glm::vec3& p_plane, const glm::vec3& v_line, const glm::vec3& p_line, glm::vec3& p_intersect)
{
    // https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
    const float numerator = glm::dot(p_plane-p_line, n_plane);
    if (numerator == 0)
        return false;

    const float denominator = glm::dot(v_line, n_plane);
    const float d = numerator/denominator;
    p_intersect = p_line+v_line*d;

    return true;
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

static void rotateAroundPoint(const glm::vec3& p_orig_world, const glm::mat3& r_rot, glm::vec3& p_rot_world)
{
    glm::mat4 t_rot_world(1.0f);
    setMat4Translation(t_rot_world, p_rot_world);

    auto v_rotToOrig_world = p_orig_world - p_rot_world;
    const auto t_world_rot = glm::inverseTranspose(t_rot_world);   
    const auto v_rotToOrig_rot = t_world_rot * glm::vec4(v_rotToOrig_world, 0.0f);

    setMat4Translation(t_rot_world, p_rot_world + v_rotToOrig_world);
    t_rot_world = glm::mat4(r_rot) * t_rot_world;

    v_rotToOrig_world = getMat4AxisX(t_rot_world)*v_rotToOrig_rot.x + getMat4AxisY(t_rot_world)*v_rotToOrig_rot.y + getMat4AxisZ(t_rot_world)*v_rotToOrig_rot.z;
    p_rot_world = getMat4Translation(t_rot_world) - v_rotToOrig_world;
}

static void rotateAroundPoint(const glm::vec3& p_orig_world, const glm::mat3& r_rot, glm::mat4& t_rot_world)
{
    auto p_rot_world = getMat4Translation(t_rot_world);

    auto v_rotToOrig_world = p_orig_world - p_rot_world;
    const auto t_world_rot = glm::inverseTranspose(t_rot_world);   
    const auto v_rotToOrig_rot = t_world_rot * glm::vec4(v_rotToOrig_world, 0.0f);

    setMat4Translation(t_rot_world, p_rot_world + v_rotToOrig_world);
    t_rot_world = glm::mat4(r_rot) * t_rot_world;

    v_rotToOrig_world = getMat4AxisX(t_rot_world)*v_rotToOrig_rot.x + getMat4AxisY(t_rot_world)*v_rotToOrig_rot.y + getMat4AxisZ(t_rot_world)*v_rotToOrig_rot.z;
    p_rot_world = getMat4Translation(t_rot_world) - v_rotToOrig_world;
    setMat4Translation(t_rot_world, p_rot_world);
}

static bool pointInTriangle(const glm::vec3& n_plane, const glm::vec3& p_plane, std::array<glm::vec3, 3> p_tri, glm::vec3 p_check)
{
    constexpr glm::vec3 UNIT_Z(0.0f, 0.0f, 1.0f);

    if (glm::any(glm::isnan(n_plane)) || glm::any(glm::isnan(p_plane)))
        return false;

    if (glm::all(glm::epsilonEqual(p_tri[0], p_tri[1], 0.001f)) || glm::all(glm::epsilonEqual(p_tri[1], p_tri[2], 0.001f)) || glm::all(glm::epsilonEqual(p_tri[2], p_tri[0], 0.001f)))
        return false;

    glm::mat3 r(1.0f);
    if (!glm::all(glm::epsilonEqual(n_plane, UNIT_Z, glm::epsilon<float>())) && !glm::all(glm::epsilonEqual(n_plane, -UNIT_Z, glm::epsilon<float>())))
        r = rotationVecToVec(n_plane, UNIT_Z);

    rotateAroundPoint(p_plane, r, p_check);

    std::array<glm::vec3, 3> V, V_;
    for (size_t i = 0; i < 3; ++i) {
        rotateAroundPoint(p_plane, r, p_tri[i]);
        V[i] = glm::vec3(glm::vec2(p_tri[i]), 1.0f);
        V_[i] = glm::vec3(glm::vec2(p_tri[i]), 0.0f);
    }

    const auto P = glm::vec3(glm::vec2(p_check), 1.0f);
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

static glm::mat3 rotX(const float angle)
{
    return glm::mat3(
        1.0f,        0.0f,         0.0f,
        0.0f, cosf(angle), -sinf(angle),
        0.0f, sinf(angle),  cosf(angle)
    );
}

static glm::mat3 rotY(const float angle)
{
    return glm::mat3(
         cosf(angle), 0.0f, sinf(angle),
         0.0f       , 1.0f,        0.0f,
        -sinf(angle), 0.0f, cosf(angle)
    );
}

static glm::mat3 rotZ(const float angle)
{
    return glm::mat3(
        cosf(angle), -sinf(angle), 0.0f,
        sinf(angle),  cosf(angle), 0.0f,
        0.0f       ,  0.0f       , 1.0f
    );
}

static glm::mat3 angleAxisF(const float angle, glm::vec3 v_axis)
{
    v_axis = glm::normalize(v_axis);
    const auto c = cosf(angle);
    const auto s = sinf(angle);
    const auto t1 = 1-c;
    return glm::mat3(
        c + v_axis[0]*v_axis[0]*t1          , v_axis[0]*v_axis[1]*t1 - v_axis[2]*s, v_axis[0]*v_axis[2]*t1 + v_axis[1]*s,
        v_axis[1]*v_axis[0]*t1 + v_axis[2]*s, c + v_axis[1]*v_axis[1]*t1          , v_axis[1]*v_axis[2]*t1 - v_axis[0]*s,
        v_axis[2]*v_axis[0]*t1 - v_axis[1]*s, v_axis[2]*v_axis[1]*t1 + v_axis[0]*s,           c + v_axis[2]*v_axis[2]*t1
    );
}

static glm::mat3 eulerXYZ(const glm::vec3& xyz)
{
    return rotZ(xyz.z)*rotY(xyz.y)*rotX(xyz.x);
}

static glm::mat3 eulerZYX(const glm::vec3& zyx)
{
    return rotX(zyx[2])*rotY(zyx[1])*rotZ(zyx[0]);
}

static glm::mat3 eulerXYZext(const glm::vec3& xyz)
{
    return rotX(xyz.x)*rotY(xyz.y)*rotZ(xyz.z);
}

static glm::mat3 eulerZYXext(const glm::vec3& zyx)
{
    return rotZ(zyx[0])*rotY(zyx[1])*rotX(zyx[2]);
}

static float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static glm::vec3 strToVec3(const std::string& str)
{
    const auto parts = splitString(str, ",; ");
    assert(parts.size() == 3 && "Wrong number of vector elemets");
    return glm::vec3(std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]));
}
