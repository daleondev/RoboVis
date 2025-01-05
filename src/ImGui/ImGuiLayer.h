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
    inline static bool getFramesActive() { return s_framesActive; }

    inline static std::pair<uint16_t, uint16_t> getViewportSize() { return s_viewportSize; }

private:
    static void dockSpace(const std::function<void(const ImGuiID)>& dockspaceContent);
    static void settings(const ImGuiID dockspaceId);
    static void viewport(const ImGuiID dockspaceId);

    static std::vector<std::tuple<float, float, float>> s_sliders;
    static bool s_bbActive;
    static bool s_framesActive;
    
    static std::pair<uint16_t, uint16_t> s_viewportSize;

    
};