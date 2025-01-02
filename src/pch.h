// glfw
#include <GLFW/glfw3.h>

// glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/glm.hpp>

// assimp
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

// imgui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// std
#include <map>
#include <ctime>
#include <queue>
#include <array>
#include <tuple>
#include <mutex>
#include <regex>
#include <chrono>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <ranges>
#include <variant>
#include <cassert>
#include <cstdint>
#include <csignal>
#include <sstream>
#include <fstream>
#include <optional>
#include <iostream>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <source_location>
#include <condition_variable>

#ifdef __cpp_lib_format
    #include <format>
#endif