#pragma once
#include "ThirdParty/ThirdPartyLibs.h"

constexpr static ImVec2 DefaultEditorWindowSize{ 400.f, 600.f };

constexpr static ImGuiTreeNodeFlags DefaultCollapsingHeaderFlags = 
	ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick | 
	ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow;


namespace CrStyles
{
	//0.35 should be around green.
	static void ButtonStyleStart(float Hue = .35f)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(Hue, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(Hue, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(Hue, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(.66f, 1.f, 0.1f));
	}


	static void ButtonStyleEnd()
	{
		ImGui::PopStyleColor(4);
		
		ImGui::PopStyleVar(2);
	}
}