#pragma once

#include "Scene/Scene.h"

#include "Util/EdgeDetector.h"

class ScenePanel
{
public:
    static void render(const ImGuiID dockspaceId);

private:
    static void entityNode(Scene::SceneEntityIterator& entityIterator);
    static void subEntityNode(const Entity entity);
    static void components(Entity entity);

    template<typename T, typename UIFunction>
	static void drawComponent(const std::string& name, Entity entity, UIFunction uiFunction, const bool removable = true);

    template<typename T>
    static void displayAddComponentEntry(const std::string& entryName);

    static Entity s_selected;
    static EdgeDetector<bool> s_trajSlider;
};