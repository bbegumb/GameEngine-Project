#pragma once

#include <glm/glm.hpp>
#include "renderer/Colourpickingrenderer.h"

class Entity;
class Scene;

class EditorSelectionController {
public:
	static EditorSelectionController& getInstance() {
		static EditorSelectionController instance;
		return instance;
	}
	ColourPickingRenderer& getColourPicker() { return picker; }

    static void selectEntityFromViewport(
        float mouseX,
        float mouseY,
        float viewportWidth,
        float viewportHeight
    );

private:
	EditorSelectionController() = default;
	ColourPickingRenderer picker;
};
