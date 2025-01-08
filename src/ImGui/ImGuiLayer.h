#pragma once

#include "Timestep.h"

#include "Events/Event.h"

#include "Util/EdgeDetector.h"

class ImGuiLayer
{
public:
    static void init();
    static void shutdown();

    static void render(const Timestep dt);

    static void onEvent(Event& e);

    static glm::vec2 screenToViewport(const glm::vec2& screenPos);
    inline static std::pair<uint16_t, uint16_t> getViewportSize() { return s_viewportSize; }
    inline static bool isViewportHovered() { return s_viewportHovered; }
    inline static bool isViewportFocused() { return s_viewportFocused; }

private:
    static void dockSpace(const std::function<void(const ImGuiID)>& dockspaceContent);
    static void viewport(const ImGuiID dockspaceId);
    static void robotControls(const ImGuiID dockspaceId);
   
    static std::pair<uint16_t, uint16_t> s_viewportSize;
    static glm::vec2 s_viewportPos;
    static bool s_viewportHovered;
    static bool s_viewportFocused;

    static EdgeDetector<float> m_sliderTime;
    static EdgeDetector<bool> m_buttonPlay;

};