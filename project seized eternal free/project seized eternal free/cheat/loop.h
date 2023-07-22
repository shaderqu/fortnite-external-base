#pragma once
#pragma warning(disable : 4996)
extern void preform_aimbot(double closest_distance, uintptr_t closest_player);

// Function to loop through the player cache and update the list of Actors
bool PlayerCache() {
	for (;;) {
		std::vector<Actors> ActorTempList;
		Pointers->UnrealWorld = driver.read<uint64_t>(defines::base_address + offsets::Uworld);
		debug_ptr((u)Pointers->UnrealWorld, _("uworld"));
		if (!(u)Pointers->UnrealWorld) return false;

		Pointers->GameInstance = driver.read<uint64_t>(Pointers->UnrealWorld + offsets::OwningGameInstance);
		debug_ptr((u)Pointers->GameInstance, _("game_instance"));
		if (!(u)Pointers->GameInstance) return false;

		Pointers->ULevel = driver.read<uint64_t>(Pointers->UnrealWorld + offsets::PersistentLevel);
		debug_ptr((u)Pointers->ULevel, _("ulevel"));

		Pointers->LocalPlayers = driver.read<uint64_t>(Pointers->GameInstance + offsets::LocalPlayers);
		debug_ptr((u)Pointers->LocalPlayers, _("local_players"));
		if (!(u)Pointers->LocalPlayers) return false;

		Pointers->LocalPlayer = driver.read<uint64_t>(Pointers->LocalPlayers);
		debug_ptr((u)Pointers->LocalPlayer, _("local_player"));
		if (!(u)Pointers->LocalPlayer) return false;

		Pointers->PlayerController = driver.read<uint64_t>(Pointers->LocalPlayer + offsets::PlayerController);
		debug_ptr((u)Pointers->PlayerController, _("player_controller"));
		if (!(u)Pointers->PlayerController) return false;

		Pointers->AcknowledgedPawn = driver.read<uint64_t>(Pointers->PlayerController + offsets::AcknowledgedPawn);
		debug_ptr((u)Pointers->AcknowledgedPawn, _("acknowledged_pawn"));

		Pointers->PlayerState = driver.read<uint64_t>(Pointers->AcknowledgedPawn + offsets::PlayerState);
		debug_ptr((u)Pointers->PlayerState, _("player_state"));

		Pointers->RootComponent = driver.read<uint64_t>(Pointers->AcknowledgedPawn + offsets::RootComponent);
		debug_ptr((u)Pointers->RootComponent, _("root_component"));

		Pointers->RelativeLocation = driver.read<vector3>(Pointers->RootComponent + offsets::RelativeLocation);
		debug_ptr((u)Pointers->RootComponent, _("relative_location"));

		Pointers->TeamIndex = driver.read<int>(Pointers->PlayerState + offsets::TeamID);
		debug_ptr((u)Pointers->TeamIndex, _("team_index"));

		Pointers->ActorArray = driver.read<int>(Pointers->ULevel + 0xA0);
		debug_ptr((u)Pointers->ActorArray, _("actor_array"));

		Pointers->Actors = driver.read<uintptr_t>(Pointers->ULevel + 0x98);
		debug_ptr((u)Pointers->Actors, _("actors"));

		// Loop through the ActorArray and populate the ActorTempList
		for (int i = 0; i < Pointers->ActorArray; ++i) {
			auto current_actor = driver.read<uint64_t>(Pointers->Actors + i * 0x8);

			if (driver.read<float>(current_actor + offsets::ReviveFromDBNOTime) == 10) {
				Actors FortniteActor{ };
				FortniteActor.Actor = current_actor;
				FortniteActor.SkeletalMesh = driver.read<uintptr_t>(current_actor + offsets::Mesh);
				FortniteActor.bIsKnocked = (driver.read<char>(current_actor + 0x872) >> 4) & 1;; // bIsDBNO
				FortniteActor.PlayerState = driver.read<uint64_t>(current_actor + offsets::PlayerState);
				FortniteActor.TeamIndex = driver.read<uintptr_t>(FortniteActor.PlayerState + offsets::TeamID);
				ActorTempList.push_back(FortniteActor);
			}
		}
		ActorList.clear();
		ActorList = ActorTempList;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void ActorLoop() {
	float closestDistance = FLT_MAX;
	DWORD_PTR closestPawn = NULL;

	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	// Draw FPS on the screen if enabled in settings
	if (settings::misc::draw_fps) {
		char fps[256]; // 64
		sprintf_s(fps, _("pasting.club | fps: %.3f"), ImGui::GetIO().Framerate);
		DrawString(16, 70, 100, ImColor(255, 255, 255, 255), false, true, fps);
	}
	// Draw FOV circle on the screen if enabled in settings
	if (settings::combat::show_fov) {
		draw_list->AddCircle(ImVec2(screen_width / 2, screen_height / 2), (settings::combat::fov * 10) - ((fov - 80) * 2.5) - 1, ImColor(20, 0, 0, 255), 99, 1);
		draw_list->AddCircle(ImVec2(screen_width / 2, screen_height / 2), (settings::combat::fov * 10) - ((fov - 80) * 2.5), ImColor(255, 255, 255, 255), 99, 1);
		draw_list->AddCircle(ImVec2(screen_width / 2, screen_height / 2), (settings::combat::fov * 10) - ((fov - 80) * 2.5) + 1, ImColor(20, 0, 0, 255), 99, 1);
	}

	// Loop through the ActorList
	for (Actors Actor : ActorList) {
		vector3 head_3d = get_bone(Actor.SkeletalMesh, 68);
		vector3 head_2d = w2s(vector3(head_3d.x, head_3d.y, head_3d.z + 15));
		debug_3d(head_2d, "head_2d");

		vector3 base_3d = get_bone(Actor.SkeletalMesh, 0);
		vector3 base_2d = w2s(base_3d);

		float box_hight = abs(head_2d.y - base_2d.y);
		float box_width = box_hight * 0.50;

		if (settings::visuals::esp) {

			ImColor visible_color;
			if (is_visible(Actor.SkeletalMesh)) {
				visible_color = ImColor(0, 255, 0, 255); // Green for visible
			}
			else {
				visible_color = ImColor(255, 0, 0, 255); // Red for non-visible
			}

			if (settings::visuals::draw_box) {
				draw_list->AddRect(ImVec2(head_2d.x - box_width / 2, head_2d.y), ImVec2((head_2d.x - box_width / 2) + box_width, head_2d.y + box_hight), visible_color, 0, 0, 1);
			}
		}

		// Calculate the distance of the player from the crosshair
		auto dist = GetCrossDistance(head_2d.x, head_2d.y, screen_width / 2, screen_height / 2);
		// Check if the player is within the FOV and is the closest so far
		if (dist < (settings::combat::fov * 10) - ((fov - 80) * 2.5) && dist < closestDistance) {
			closestDistance = dist;
			closestPawn = Actor.Actor;
		}
	}

	// If the right mouse button is held down, perform aimbot
	if (GetAsyncKeyState(VK_RBUTTON))
		preform_aimbot(closestDistance, closestPawn);
}