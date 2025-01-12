#include "pch.h"

#include "ScenePanel.h"

#include "Scene/Components.h"

Entity ScenePanel::s_selected = EntityNull;
EdgeDetector<bool> ScenePanel::s_trajSlider;

void ScenePanel::render(const ImGuiID dockspaceId)
{
    ImGui::Begin("Entities");
    for (auto it = Scene::s_entities.begin(); it != Scene::s_entities.end(); )
        entityNode(it);
    ImGui::End();

    ImGui::Begin("Properties");
    if (s_selected)
        components(s_selected);
    ImGui::End();
}

void ScenePanel::entityNode(Scene::SceneEntityIterator& it)
{
    const auto& [id, entity] = *it;
    const auto& tag = entity.getTag();

    // check if entity should be displayed
    if (!entity.hasComponent<PropertiesComponent>()) {
        ++it;
        return;
    }
    const auto& properties = entity.getComponent<PropertiesComponent>();
    if (!properties.copyable && !properties.editable && !properties.removable && !entity.hasComponent<JointComponent>()) {
        ++it;
        return;
    }

    // Display Entity as Node
    ImGuiTreeNodeFlags flags = (s_selected == entity ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None) | (entity.hasComponent<JointComponent>() ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf);
    bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(id), flags, entity.getTag().c_str());
    if (ImGui::IsItemClicked()) {
        s_selected = entity;
    }

    // Popup for delete
    bool deleteEntity = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (properties.removable && ImGui::MenuItem("Delete"))
            deleteEntity = true;

        ImGui::EndPopup();
    }

    // Sub Nodes
    if (open) {
        if (entity.hasComponent<JointComponent>()) {
            const auto& joint = entity.getComponent<JointComponent>();
            if (Scene::s_registry.valid(joint.childLink)) {
                subEntityNode(joint.childLink);
            }
        }

        ImGui::TreePop();
    }

    // delete if requested
    if (deleteEntity) {
        it = Scene::destroyEntity(it);
        if (s_selected == it->second)
            s_selected = EntityNull;
    }
    else
        ++it;
}

void ScenePanel::subEntityNode(const Entity entity)
{
    const auto& id = entity.getId();
    const auto& tag = entity.getTag();

    ImGuiTreeNodeFlags flags = (s_selected == entity ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None) | (entity.hasComponent<JointComponent>() ? ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf);
    bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(id), flags, entity.getTag().c_str());
    if (ImGui::IsItemClicked()) {
        s_selected = entity;
    }

    // Popup for delete
    bool deleteEntity = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (entity.hasComponent<PropertiesComponent>() && entity.getComponent<PropertiesComponent>().removable && ImGui::MenuItem("Delete"))
            deleteEntity = true;

        ImGui::EndPopup();
    }

    // delete if requested
    if (deleteEntity) {
        Scene::destroyEntity(entity);
        if (s_selected == entity)
            s_selected = EntityNull;
        return;
    }

    if (open) {
        if (entity.hasComponent<JointComponent>()) {
            const auto& joint = entity.getComponent<JointComponent>();
            if (Scene::s_registry.valid(joint.childLink)) {
                subEntityNode(joint.childLink);
            }
        }

        ImGui::TreePop();
    }
}

void ScenePanel::components(Entity entity)
{
    // Tag
    if (entity.hasComponent<TagComponent>()) {
        auto& tag = entity.getComponent<TagComponent>().tag;

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy(buffer, tag.c_str(), sizeof(buffer));
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            tag = std::string(buffer);
    }

    // Add new components
    if (!entity.hasComponent<PropertiesComponent>() || entity.getComponent<PropertiesComponent>().editable) {
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
    
        if (ImGui::Button("+"))
            ImGui::OpenPopup("+");

        if (ImGui::BeginPopup("+"))
        {
            displayAddComponentEntry<PropertiesComponent>("Properties");
            if (entity.hasComponent<PropertiesComponent>() && entity.getComponent<PropertiesComponent>().editable) {
                displayAddComponentEntry<TransformComponent>("Transform");
                displayAddComponentEntry<TriangulationComponent>("Triangulation");   
                displayAddComponentEntry<BoundingBoxComponent>("Bounding Box");
            }

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();
    }

    // check properties
    if (!entity.hasComponent<PropertiesComponent>())
        return;
    auto& properties = entity.getComponent<PropertiesComponent>();

    // properties
    drawComponent<PropertiesComponent>("Properties", entity, [](auto& properties) {
        const auto contentRegionAvail = ImGui::GetContentRegionAvail();

        ImGui::Checkbox("Visible", &properties.visible);
        ImGui::SameLine(0.5f*contentRegionAvail.x);
        ImGui::SetNextItemWidth(0.45f*contentRegionAvail.x);
        ImGui::Checkbox("Clickable", &properties.clickable);
        ImGui::Separator();

        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::Checkbox("Editable", &properties.editable);
        ImGui::SameLine(0.5f*contentRegionAvail.x);
        ImGui::SetNextItemWidth(0.45f*contentRegionAvail.x);
        ImGui::Checkbox("Removable", &properties.removable);
        ImGui::Checkbox("Copyable", &properties.copyable);
        ImGui::PopItemFlag();
    }, false);

    // transform
    drawComponent<TransformComponent>("Transform", entity, [&properties](auto& transform) {
        if (!properties.editable)
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);

        ImGui::DragFloat3("Translation", glm::value_ptr(transform.translation), 0.1f);
        ImGui::Separator();
        ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 0.1f);
        ImGui::Separator();
        ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.1f);

        if (!properties.editable)
            ImGui::PopItemFlag();
    }, false);

    // robot
    drawComponent<RobotComponent>("Robot", entity, [&entity](auto& robot) {
        const auto contentRegionAvail = ImGui::GetContentRegionAvail();

        // properties
        ImGui::Checkbox("Draw Frames", &(bool&)robot.drawFrames);
        if (robot.drawFrames) {
            ImGui::SameLine(0.5f*contentRegionAvail.x);
            ImGui::SetNextItemWidth(0.4f*contentRegionAvail.x);
            ImGui::DragFloat("Scale", &(float&)robot.frameScale, 0.01f, 0.01f, 1.0f);
        }
        ImGui::Checkbox("Draw Bounding Boxes", &(bool&)robot.drawBoundingBoxes);
        if (robot.drawBoundingBoxes) {
            ImGui::SameLine(0.5f*contentRegionAvail.x);
            ImGui::SetNextItemWidth(0.4f*contentRegionAvail.x);
            ImGui::ColorEdit4("Color", glm::value_ptr((glm::vec4&)robot.boundingBoxColor));
        }
        ImGui::Separator();

        // joint values
        ImGui::Text("Joint values:");
        for (JointComponent& joint : robot.joints | std::views::reverse) {
            ImGui::SliderAngle(joint.name.c_str(), &joint.value, rad2deg(joint.limits[0]), rad2deg(joint.limits[1]));
        }
        ImGui::Separator();

        if (robot.trajectory) {       
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

            ImGui::Text("Trajectory");

            ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
            if (ImGui::Button(("-##" + std::to_string(typeid(robot.trajectory).hash_code())).c_str(), ImVec2{ lineHeight, lineHeight }))
                ImGui::OpenPopup("TrajectorySettings");

            if (ImGui::BeginPopup("TrajectorySettings")) {
                if (ImGui::MenuItem("Remove trajectory"))
                    robot.trajectory = {};

                ImGui::EndPopup();
            }
            if (!robot.trajectory)
                return;

            auto& trajectory = *robot.trajectory;
            ImGui::SetNextItemWidth(0.94 * ImGui::GetCurrentWindow()->Size.x);
            ScenePanel::s_trajSlider = ImGui::SliderFloat("##Traj", &trajectory.currentTime, trajectory.times.front(), trajectory.times.back());	

            if (trajectory.currentIndex == 0) trajectory.currentIndex = 1;
            if (s_trajSlider().edge() && !trajectory.active && trajectory.currentIndex > 0 && trajectory.currentIndex < trajectory.jointValues.size()) {
                for (size_t i = 0; i < robot.joints.size(); ++i) {           
                    robot.joints[robot.joints.size()-i-1].get().value = map(
                        trajectory.currentTime, 
                        trajectory.times[trajectory.currentIndex-1], 			trajectory.times[trajectory.currentIndex], 
                        trajectory.jointValues[trajectory.currentIndex-1][i], trajectory.jointValues[trajectory.currentIndex][i]); 
                } 
            }

            // m_buttonPlay.val() = ImGui::ImageButton("play", TextureLibrary::get("image")->getId(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0));
            // if (m_buttonPlay().rising()) {
            //     trajectory.active = !trajectory.active;
            // }
        }
    }, false);

    // joint
    drawComponent<JointComponent>("Joint", entity, [&entity](auto& joint) {
        ImGui::Text("Type: %s", JointComponent::typeToStr(joint.type));
        if (joint.type == JointComponent::Type::Revolute)
            ImGui::SliderAngle("Joint value", &joint.value, rad2deg(joint.limits[0]), rad2deg(joint.limits[1]));
    }, properties.editable && entity.hasComponent<JointComponent>() && (entity.getComponent<JointComponent>().type != JointComponent::Type::Fixed));

    // triangulation
    drawComponent<TriangulationComponent>("Triangulation", entity, [&properties](auto& triangulation) {
        ImGui::Text("Num vertices: %d", triangulation.data->vertices.size());
        ImGui::Text("Num indices: %d", triangulation.data->indices.size());
    }, properties.editable);

    // bounding box
    drawComponent<BoundingBoxComponent>("Bounding Box", entity, [&properties](auto& boundingBox) {
        ImGui::Checkbox("Visible", &boundingBox.visible);
    }, properties.editable);
}

template<typename T, typename UIFunction>
void ScenePanel::drawComponent(const std::string& name, Entity entity, UIFunction uiFunction, const bool removable)
{
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
    if (entity.hasComponent<T>())
    {
        auto& component = entity.getComponent<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
        ImGui::PopStyleVar();

        bool removeComponent = false;
        if (removable) {
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button(("-##" + std::to_string(typeid(T).hash_code())).c_str(), ImVec2{ lineHeight, lineHeight }))
            {
                ImGui::OpenPopup(("ComponentSettings##" + std::to_string(typeid(T).hash_code())).c_str());
            }

            if (ImGui::BeginPopup(("ComponentSettings##" + std::to_string(typeid(T).hash_code())).c_str()))
            {
                if (ImGui::MenuItem(("Remove component##" + std::to_string(typeid(T).hash_code())).c_str()))
                    removeComponent = true;

                ImGui::EndPopup();
            }
        }

        if (open)
        {
            uiFunction(component);
            ImGui::TreePop();
        }

        if (removeComponent)
            entity.RemoveComponent<T>();
    }
}

template<typename T>
void ScenePanel::displayAddComponentEntry(const std::string& entryName) {
    if (!s_selected.hasComponent<T>()) {
        if (ImGui::MenuItem(entryName.c_str())) {
            s_selected.addComponent<T>();
            ImGui::CloseCurrentPopup();
        }
    }
}