#pragma once

#include <reflection/Reflection.h>
#include <scene/components/Component.h>
#include <imgui.h>
#include <glm/glm.hpp>

inline void DrawReflectedFields(Component* component, TypeInfo* type){
    if (!component || !type) return;

    for (FieldInfo& field : type->fields){
        void* fieldAddress = field.getAddress(component);

        switch (field.type){
            case FieldType::Float:
                ImGui::DragFloat(field.name.c_str(), static_cast<float*>(fieldAddress), 0.01f);
                break;

            case FieldType::Int:
                ImGui::DragInt(field.name.c_str(), static_cast<int*>(fieldAddress));
                break;

            case FieldType::Bool:
                ImGui::Checkbox(field.name.c_str(), static_cast<bool*>(fieldAddress));
                break;

            case FieldType::Vec3:
            {
                glm::vec3* value = static_cast<glm::vec3*>(fieldAddress);
                ImGui::DragFloat3(field.name.c_str(), &value->x, 0.1f);
                break;
            }

            case FieldType::Unknown:
                ImGui::Text("%s: unsupported type", field.name.c_str());
                break;
        }
    }
}
