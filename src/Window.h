#pragma once

#include "Events/Event.h"

#include <GLFW/glfw3.h>

class Window
{
public:
    static void create(const std::string& title, const uint16_t width, const uint16_t height);
    static void shutdown();

    static void onUpdate();

    inline static uint32_t getWidth() { return s_data.width; }
    inline static uint32_t getHeight() { return s_data.height; }

    inline static void setEventCallback(const EventCallbackFunction& callback) { s_data.eventCallback = callback; }

    inline static GLFWwindow* getNativeWindow() { return s_window; }

    static void setVSync(const bool enabled);
	static bool getVSync();

    inline static bool isInitialized() { return s_initialized; }

private:
    static void init();

    struct WindowData {
        std::string title;
        uint16_t width;
        uint16_t height;
        bool vSync;

        EventCallbackFunction eventCallback;
    };
    static WindowData s_data;

    static GLFWwindow* s_window;
    static bool s_initialized;
};