#pragma once

#include "Timestep.h"

class ImGuiLayer
{
public:
    static void init();
    static void shutdown();

    static void render(const Timestep dt);

    inline static void addSlider(const float value, std::pair<float, float>& limits) { s_sliders.push_back({value, limits.first, limits.second}); }
    inline static float getSliderValue(const size_t idx) { return std::get<0>(s_sliders[idx]); }
    inline static bool getBoundingBoxesActive() { return s_bbActive; }

private:
    static std::vector<std::tuple<float, float, float>> s_sliders;
    static bool s_bbActive;

    static void dockSpace();
    static void settings(const Timestep dt);
};