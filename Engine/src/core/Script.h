#pragma once

#include <scene/components/BehaviourComponent.h>
#include <persistance/ComponentFactory.h>

#define SCRIPT(ClassName) \
    class ClassName : public BehaviourComponent

#define END_SCRIPT(ClassName) \
    inline bool _autoReg_##ClassName = []() { \
        ComponentFactory::registerType(#ClassName, [](Entity& e, const nlohmann::json& j) { \
            auto& c = e.addComponent<ClassName>(); \
            c.deserialize(j); \
        }); \
        return true; \
    }()