#include "Application.h"

#include <memory>

#include "geometry.h"

int main(int argc, char **argv) 
{
    // // glm::vec3 n (0.5, 0.2, 0.3);
    // // n = normalize(n);

    // // glm::vec3 v2 (0.0, 0.0, 1.0);

    // // const auto R = rotationVecToVec(n, v2);
    // // const auto euler = glm::eulerAngles(glm::toQuat(R));
    // // std::cout << euler[0] << ", " << euler[1]  << ", " << euler[2]  << "\n";

    // // glm::vec3 w = n;
    // // glm::vec3 v = glm::normalize(glm::cross(w, {1,0,0}));
    // // glm::vec3 u = glm::normalize(glm::cross(w, v));

    // // glm::mat4 P(1.0f);
    // // P[0][0] = u[0];
    // // P[1][0] = u[1];
    // // P[2][0] = u[2];
    // // P[0][1] = v[0];
    // // P[1][1] = v[1];
    // // P[2][1] = v[2];
    // // P[0][2] = w[0];
    // // P[1][2] = w[1];
    // // P[2][2] = w[2];

    // // P = P*glm::mat4(R);
    // // std::cout << P[0][2] << ", " << P[1][2]  << ", " << P[2][2]  << "\n";

    // std::array<glm::vec3, 3> v;
    // v[0] = { 10, 5, -12.4};
    // v[1] = { 20, 5, 22.4};
    // v[2] = { 15, 10, 0.2};

    // glm::vec3 l0 (15, 12, -100);
    // glm::vec3 l (0, 0, 1);

    // const auto&[n, p0] = trianglePlane(v);
    // auto i = intersectionLinePlane(n, p0, l, l0);

    // // auto l1 = v[1] - v[0];
    // // auto l2 = v[2] - v[1];
    // // auto l3 = v[0] - v[2];

    // // std::cout << location(l1, p) << std::endl;
    // // std::cout << location(l2, p) << std::endl;
    // // std::cout << location(l3, p) << std::endl;

    // std::cout << pointInTriangle(n, p0, v, *i) << std::endl;

    // std::vector<std::array<int, 2>> data(2);
    // data[0][0] = 24;
    // data[0][1] = -100;
    // data[1][0] = 13;
    // data[1][1] = 10321;

    // int arr[4];
    // memcpy(arr, data.data()->data(), sizeof(arr));

    // for (int i = 0;i < 4; ++i)
    //     std::cout << arr[i] << std::endl;

    // glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);  
    // glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
    // glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    // glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    // glm::mat4 view;
    // view = glm::lookAt( cameraPos, 
    //                     cameraTarget, 
    //                     cameraUp);

    // printMat(view);

    // glm::mat4 t = glm::translate(glm::mat4(1.0f), {1, 0, 5});
    // printMat(t);

    // glm::vec3 p(1, 2, 3);
    // p = t*glm::vec4(p, 1.0f);

    // std::cout << p.x << ", " << p.y << ", " << p.z << "\n";

    auto app = std::make_unique<Application>();
    return app->run(argc, argv);
}