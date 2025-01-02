#pragma once

#include "Timestep.h"

class ImGuiLayer
{
public:
    static void init();
    static void shutdown();

    static void render(const Timestep dt);

private:
    static void dockSpace();
    static void settings(const Timestep dt);
};