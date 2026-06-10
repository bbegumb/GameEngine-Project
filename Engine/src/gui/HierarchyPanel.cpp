#include "HierarchyPanel.h"

#include <scene/Entity.h>
#include <scene/Scene.h>
#include <scene/components/TransformComponent.h>
#include <imgui.h>
#include <algorithm>

const char* HierarchyPanel::DND_ID = "HIERARCHY_ENTITY";

void HierarchyPanel::drawNode(Entity* e, const std::vector<std::unique_ptr<Entity>>& allEntities) {
    std::vector<Entity*> children;
    for (auto& other : allEntities) {
        if (!other) continue;
        TransformComponent* p = other->transform.getParent();
        if (p && p == &e->transform)
            children.push_back(other.get());
    }

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (getSelectedEntity && getSelectedEntity() == e)
        flags |= ImGuiTreeNodeFlags_Selected;

    ImGui::PushID(e);

    if (renamingEntity == e) {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##rename", renameBuffer, sizeof(renameBuffer),
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
            if (onRenameEntity && strlen(renameBuffer) > 0)
                onRenameEntity(e, std::string(renameBuffer));
            renamingEntity = nullptr;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            renamingEntity = nullptr;

        ImGui::PopID();
        return;
    }

    bool open = ImGui::TreeNodeEx((void*)(intptr_t)e, flags, "%s", e->getName().c_str());

    if (ImGui::IsItemClicked() && onEntitySelected)
        onEntitySelected(e);

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetDragDropPayload(DND_ID, &e, sizeof(Entity*));
        ImGui::Text("Move: %s", e->getName().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* pl = ImGui::AcceptDragDropPayload(DND_ID)) {
            Entity* dragged = *(Entity**)pl->Data;
            bool isCycle = false;
            TransformComponent* check = &e->transform;
            while (check) {
                if (check == &dragged->transform) { isCycle = true; break; }
                check = check->getParent();
            }
            if (!isCycle && dragged != e)
                dragged->transform.setParent(&e->transform);
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginPopupContextItem("##entityCtx")) {
        if (ImGui::MenuItem("Rename")) {
            renamingEntity = e;
            strncpy(renameBuffer, e->getName().c_str(), sizeof(renameBuffer) - 1);
            renameBuffer[sizeof(renameBuffer) - 1] = '\0';
        }
        if (e->transform.getParent()) {
            if (ImGui::MenuItem("Unparent"))
                e->transform.setParent(nullptr);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Delete")) {
            if (onDeleteEntity) onDeleteEntity(e);
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            if (open) ImGui::TreePop();
            ImGui::PopID();
            return;
        }
        ImGui::EndPopup();
    }

    if (open) {
        for (Entity* child : children)
            drawNode(child, allEntities);
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void HierarchyPanel::show(Scene* scene, bool* open) {
    if (!ImGui::Begin("Hierarchy", open)) {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Add Entity") && onCreateEntity)
        onCreateEntity("New Entity");

    ImGui::Separator();

    const auto& entities = scene->getEntities();

    for (auto& entityPtr : entities) {
        Entity* e = entityPtr.get();
        if (!e || e->transform.getParent() != nullptr) continue;
        drawNode(e, entities);
    }

    ImGui::InvisibleButton("##hierarchyBg",
        ImVec2(ImGui::GetContentRegionAvail().x,
            std::max(ImGui::GetContentRegionAvail().y, 20.0f)));

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* pl = ImGui::AcceptDragDropPayload(DND_ID)) {
            Entity* dragged = *(Entity**)pl->Data;
            dragged->transform.setParent(nullptr);
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}