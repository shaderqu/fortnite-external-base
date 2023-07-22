#pragma once
#include "../../includes/includes.hpp"

static int current_tab;
inline void draw_menu() {
	ImGui::SetNextWindowSize(ImVec2(400, 250));
	ImGui::Begin(_("seizeds amazing fortnite cheat"), nullptr, ImGuiWindowFlags_NoDecoration);
	ImGui::SetCursorPos({ 15,39.f });
	if (ImGui::Button(_("Combat"), { 80, 26 }))
	{
		current_tab = 0;
	}

	ImGui::SetCursorPos({ 105,39.f });
	if (ImGui::Button(_("Visuals"), { 80, 26 }))
	{
		current_tab = 1;
	}
	ImGui::SetCursorPos({ 195,39.f });
	if (ImGui::Button(_("Misc"), { 80, 26 })) {
		current_tab = 2;
	}

	if (current_tab == 0) {
		ImGui::Checkbox(_("Enable"), &settings::combat::aimbot);
		ImGui::Checkbox(_("Draw FOV"), &settings::combat::show_fov);
		ImGui::SliderInt(_("FOV Size"), &settings::combat::fov, 1, 50);
		ImGui::SliderInt(_("Smoothing"), &settings::combat::smoothing, 1, 35);
		ImGui::PushItemWidth(150);
		static const char* bonename[] = {
			"Head",
			"Neck",
			"Chest"
		};
		ImGui::Combo(_("Hitbox"), &settings::combat::hitbox, bonename, IM_ARRAYSIZE(bonename));
	}
	if (current_tab == 1) {
		ImGui::Checkbox(_("Enable"), &settings::visuals::esp);
		ImGui::Checkbox(_("Draw Box"), &settings::visuals::draw_box);
	}
	if (current_tab == 2) {
		ImGui::Checkbox(_("Draw FPS"), &settings::misc::draw_fps);
		ImGui::Checkbox(_("VSync"), &settings::misc::vsync);
	}

	ImGui::End();
}