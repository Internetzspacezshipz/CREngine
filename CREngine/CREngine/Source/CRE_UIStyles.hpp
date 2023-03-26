#pragma once
#include <ThirdParty/imgui/imgui.h>

constexpr static ImVec2 DefaultButtonSize{ 70.f, 20.f };
constexpr static ImVec2 LargeButtonSize{ 130.f, 20.f };
constexpr static ImVec2 LargerButtonSize{ 180.f, 20.f };

constexpr static ImGuiTreeNodeFlags DefaultCollapsingHeaderFlags = 
	ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick | 
	ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow;
