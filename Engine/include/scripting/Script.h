#pragma once

#include <scene/components/BehaviourComponent.h>
#include <persistance/ComponentFactory.h>

class Archive;

#define SCRIPT(ClassName) \
    class ClassName : public BehaviourComponent

// For registering ONLY user level scripts/components
#define END_SCRIPT(ClassName) \
    inline bool _autoReg_##ClassName = []() { \
        ComponentFactory::registerScript(#ClassName, [](Entity& e, const Archive& arch) { \
            auto& c = e.addComponent<ClassName>(); \
            c.deserialize(arch); \
        }); \
        return true; \
    }()