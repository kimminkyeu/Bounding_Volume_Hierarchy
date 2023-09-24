//
// Created by Minkyeu Kim on 9/24/23.
//

#ifndef SCOOP_GUI_H
#define SCOOP_GUI_H

#include "imgui_internal.h"
#include <string>


enum class ButtonToggleFlags
{
	LayoutMode          = 0,
	TexturePaintMode    = 1,
	RenderMode          = 1 << 1,
	ModelingMode        = 1 << 2,
	SculptingMode       = 1 << 3,
};

ImVec2 GUI_PUSH_IMAGE_TO_VIEWPORT(const char* name, unsigned int texture_id, const std::shared_ptr<Lunar::EditorCamera>& cameraPtr)
{
	ImGui::Begin(name);
	ImVec2 avail = ImGui::GetContentRegionAvail();
	cameraPtr->OnResize(avail.x, avail.y);
	ImGui::Image(
			reinterpret_cast<ImTextureID>(texture_id),
			avail,
			ImVec2(0, 1),
			ImVec2(1, 0)
	);
	ImGui::End();
	return avail;
}

void GUI_PUSH_STYLE_COLOR_BY_BUTTON_STATE(ButtonToggleFlags current_state, ButtonToggleFlags target)
{
	const ImVec4 CLICKED_COLOR = ImVec4{0.4f, 0.5f, 0.8f, 1.0f};
	const ImVec4 DEFAULT_COLOR = ImVec4{0.2f, 0.3f, 0.4f, 1.0f};

	if (current_state == target) {
		ImGui::PushStyleColor(ImGuiCol_Button, CLICKED_COLOR);
	} else {
		ImGui::PushStyleColor(ImGuiCol_Button, DEFAULT_COLOR);
	}
}

//void GUI_MODE_SELECTION_BUTTON_COMPONENT(const char* name, float width, float height, ButtonToggleFlags* btnStatePtr, bool* rayTracerFlagPtr)
//{
//	GUI_PUSH_STYLE_COLOR_BY_BUTTON_STATE(*btnStatePtr, ButtonToggleFlags::LayoutMode);
//	ImGui::SameLine(0.0, 2.0f);
//	if (ImGui::Button(name, ImVec2(width, height)))
//	{
//		*btnStatePtr = ButtonToggleFlags::LayoutMode;
//		if (*btnStatePtr )
//	}
//	ImGui::PopStyleColor(1);
//}

#endif //SCOOP_GUI_H
