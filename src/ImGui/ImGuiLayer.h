#pragma once

#include "Timestep.h"

class ImGuiLayer
{
public:
    static void init();
    static void shutdown();

    static void render(const Timestep dt);

    inline static const void setLimits(const std::array<std::pair<float, float>, 6>& limits) { s_limits = limits; }
    inline static const void setLimits(const size_t idx, std::pair<float, float>& limits) { s_limits[idx] = limits; }
    inline static std::array<float, 6>& getSliders() { return s_sliders; }
    inline static float& getSlider(const size_t idx) { return s_sliders[idx]; }

private:
    static std::array<std::pair<float, float>, 6> s_limits;
    static std::array<float, 6> s_sliders;

    static void dockSpace();
    static void settings(const Timestep dt);
};