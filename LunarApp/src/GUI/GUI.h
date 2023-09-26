//
// Created by Minkyeu Kim on 9/24/23.
//

#ifndef SCOOP_GUI_H
#define SCOOP_GUI_H

#include "imgui_internal.h"
#include <string>
#include "LunarApp/src/Shaders/DataVisualizer.h"

void GUI_SET_INITIAL_UI()
{
	// 1. Now we'll need to create our dock node:

	ImGuiID id = ImGui::GetID("DockSpace"); // str_id set in application.cpp
	ImGui::DockBuilderRemoveNode(id);             // Clear any preexisting layouts associated with the ID we just chose
	ImGui::DockBuilderAddNode(id); // add node to dock full screen

	// 2. Split the dock node to create spaces to put our windows in:

	// Split the dock node in the left direction to create our first docking space. This will be on the left side of the node.
	// (The 0.5f means that the new space will take up 50% of its parent - the dock node.)
	ImGuiID dock0 = ImGui::DockBuilderSplitNode(id, ImGuiDir_Up, 0.05f, nullptr, &id);
	ImGuiID dock2 = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.2f, nullptr, &id);
	ImGuiID dock1_a = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.5f, nullptr, &id);
	ImGuiID dock1_b = ImGui::DockBuilderSplitNode(dock1_a, ImGuiDir_Left, 0.5f, nullptr, &dock1_a);
	// +-----------+
	// +-----------+
	// |     1     |
	// |           |
	// +-----------+
	// Split the same dock node in the right direction to create our second docking space.
	// At this point, the dock node has two spaces, one on the left and one on the right.
	// +-----------+
	// +-----+-----+
	// |  1  |  2  |
	// |     |     |
	// +-----+-----+
	//    split ->
	// For our last docking space, we want it to be under the second one but not under the first.
	// Split the second space in the down direction so that we now have an additional space under it.
	//
	// Notice how "dock2" is now passed rather than "id".
	// The new space takes up 50% of the second space rather than 50% of the original dock node.
	// +-----+-----+
	// |     |  2  |  split
	// |  1  +-----+    |
	// |     |  3  |    V
	// +-----+-----+

	// 3. Add windows to each docking space:
	ImGui::DockBuilderDockWindow("Mode Selection", dock0);
	ImGui::DockBuilderDockWindow("Main Viewport", dock1_a);
	ImGui::DockBuilderDockWindow("Second Viewport", dock1_b);
	ImGui::DockBuilderDockWindow("Property", dock2);

	// 4. We're done setting up our docking configuration:
	ImGui::DockBuilderFinish(id);
}

ImVec2 GUI_VIEWPORT_IMAGE_COMPONENT(const char* name, unsigned int texture_id, const std::shared_ptr<Lunar::EditorCamera>& cameraPtr, std::function<void()>&& OnImageRenderFn = {})
{
	ImGui::Begin(name);
	ImVec2 windowSize = ImGui::GetWindowSize(); // padding까지 포함된 전체 윈도우 영역.
	ImVec2 avail = ImGui::GetContentRegionAvail(); // padding 제외 순수 컨텐츠 영역.

	// GetContentRegionAvail이 첫 Frame 렌더시 정확한 값을 반환하지 않는 문제가 있음으로, 다음 프레임에서 이미지를 넣기 위함.
	if (avail.x > 0 && avail.y > 0)
	{
		cameraPtr->OnResize(avail.x, avail.y);
		ImGui::Image(
				reinterpret_cast<ImTextureID>(texture_id),
				avail,
				ImVec2(0, 1),
				ImVec2(1, 0)
		);
	}
	ImGui::End();
	return windowSize;
}

void GUI_VISUALIZER_BUTTON_COMPONENT(Option buttonType, DataVisualizer* dataVisualizerPtr, std::function<void()>&& OnClickFn = {})
{
	std::string name;

	switch (buttonType)
	{
		case Option::Grid        : name = "Grid"; break;
		case Option::AABB        : name = "AABB"; break;
		case Option::Point       : name = "Point"; break;
		case Option::Wireframe   : name = "Wireframe"; break;
		case Option::Normal      : name = "Normal"; break;
		case Option::Mesh        : name = "Mesh"; break;
		default                  : name = "Undefined";
	}

	if (ImGui::Button(name.c_str())) {
		// LAYOUT_MODE에 대한 설정을 저장함과 동시에, 해당 멤버 State를 변경.
		VS_LAYOUT_MODE_OPTION = dataVisualizerPtr->ToggleMode(buttonType);
		if (OnClickFn) OnClickFn();
	}
	ImGui::SameLine(); ImGui::Text("%s", dataVisualizerPtr->IsSet(buttonType) ? "On" : "Off");
}

enum class ButtonToggleFlags : u_char
{
	LayoutMode          = 0,
	TexturePaintMode    = 1,
	RenderMode          = 1 << 1,
	ModelingMode        = 1 << 2,
	SculptingMode       = 1 << 3,
};

void GUI_MODE_BUTTON_COMPONENT(ButtonToggleFlags buttonType, ButtonToggleFlags* statePtr, std::function<void()>&& OnClickFn = {})
{
	// Component Logic
	const ImVec4 CLICKED_COLOR = ImVec4{0.4f, 0.5f, 0.8f, 1.0f};
	const ImVec4 DEFAULT_COLOR = ImVec4{0.2f, 0.3f, 0.4f, 1.0f};
	std::string name;

	switch (buttonType)
	{
		case ButtonToggleFlags::LayoutMode        : name = "Layout"; break;
		case ButtonToggleFlags::ModelingMode      : name = "Modeling"; break;
		case ButtonToggleFlags::RenderMode        : name = "Render"; break;
		case ButtonToggleFlags::SculptingMode     : name = "Sculpting"; break;
		case ButtonToggleFlags::TexturePaintMode  : name = "Texture Paint"; break;
		default                                   : name = "Undefined";
	}

	// OnRender
	if (*statePtr == buttonType) {
		ImGui::PushStyleColor(ImGuiCol_Button, CLICKED_COLOR);
	} else {
		ImGui::PushStyleColor(ImGuiCol_Button, DEFAULT_COLOR);
	}
	ImGui::SameLine(0.0, 2.0f);
	if (ImGui::Button(name.c_str(), ImVec2(100, 20)))
	{
		*statePtr = buttonType;
		if (OnClickFn) OnClickFn();
	}
	ImGui::PopStyleColor(1);
}

#endif //SCOOP_GUI_H
